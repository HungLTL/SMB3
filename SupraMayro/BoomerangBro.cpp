#include "BoomerangBro.h"

#include "Mario.h"
#include "Boomerang.h"

#include "Game.h"
#include "PlayScene.h"

CBoomerangBro::CBoomerangBro(int x, int X) {
	min_x = x;
	max_x = X;
	mid_x = min_x + WIDTH;
	ammo = 2;

	Ready = false;

	nextState = NULL;
	idle = 0;
	idle_delay = check_start = 0;

	StartCheck();
	SetState(BBRO_STATE_WALK_RIGHT);
}

void CBoomerangBro::SetState(int state) {
	CGameObject::SetState(state);

	switch (state) {
	case BBRO_STATE_IDLE:
		vx = 0;
		StartIdle();
		break;
	case BBRO_STATE_WALK_LEFT:
		vx = -BBRO_SPEED;
		Ready = true;
		nextState = BBRO_STATE_WALK_RIGHT;
		break;
	case BBRO_STATE_WALK_RIGHT:
		vx = BBRO_SPEED;
		Ready = true;
		nextState = BBRO_STATE_WALK_LEFT;
		break;
	case BBRO_STATE_DEATH:
		vx = 0;
		vy = -BBRO_DIE_DEFLECT_SPEED;
		CGame::GetInstance()->AddScore(100);
		break;
	}
}

void CBoomerangBro::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents) {
	for (UINT i = 0; i < coObjects->size(); i++) {
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f) {
			if (dynamic_cast<CMario*>(e->obj) || dynamic_cast<CBoomerang*>(e->obj)) {
				delete e;
				continue;
			}
			else
				coEvents.push_back(e);
		}
		else
			delete e;
	}
}

void CBoomerangBro::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	float px, py = 0;
	dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->getPlayer()->GetPosition(px, py);

	if (state != BBRO_STATE_DEATH) {
		if (px < x)
			nx = -1;
		else
			nx = 1;
	}

	if (GetTickCount() - check_start >= CHECK_TIME) {
		if (ammo == 0)
			ammo++;
		check_start = 0;
		StartCheck();
	}

	if (state == BBRO_STATE_IDLE) {
		if (GetTickCount() - idle_delay >= IDLE_TIME) {
			idle = 0;
			idle_delay = 0;
			SetState(nextState);
		}
	}

	vy += BBRO_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0) {
		x += dx;
		y += dy;

		if (state == BBRO_STATE_WALK_LEFT) {
			if (x < min_x) {
				x = (float)min_x;
				SetState(BBRO_STATE_IDLE);
			}
			if (x <= mid_x) {
				if (Ready) {
					float cx, cy; CGame::GetInstance()->GetCamPos(cx, cy);
					int scrnw = CGame::GetInstance()->GetScreenWidth();
					int scrnh = CGame::GetInstance()->GetScreenHeight();
					if ((x > cx - BBRO_BBOX_WIDTH) && (y < cy + scrnh)) {
						if (ammo > 0) {
							CBoomerang* boomerang = NULL;
							if (nx < 0) {
								boomerang = new CBoomerang(false);
								boomerang->SetPosition(x + 10.0f, y - 6.0f);
							}
							else {
								boomerang = new CBoomerang(true);
								boomerang->SetPosition(x, y - 6.0f);
							}

							boomerang->GetInitBoundaries();
							boomerang->SetBoundaries();
							dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->PushObject(boomerang);
							ammo--;
						}
					}
					Ready = false;
				}
			}
		}

		if (state == BBRO_STATE_WALK_RIGHT) {
			if (x > max_x) {
				x = (float)max_x;
				SetState(BBRO_STATE_IDLE);
			}
			if (x >= mid_x) {
				if (Ready) {
					if (ammo > 0) {
						CBoomerang* boomerang = NULL;
						if (nx < 0) {
							boomerang = new CBoomerang(false);
							boomerang->SetPosition(x + 10.0f, y - 6.0f);
						}
						else {
							boomerang = new CBoomerang(true);
							boomerang->SetPosition(x, y - 6.0f);
						}

						boomerang->GetInitBoundaries();
						boomerang->SetBoundaries();
						dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->PushObject(boomerang);
						ammo--;
					}
					Ready = false;
				}
			}
		}
	}
	else {
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		x += min_tx * dx + nx * 0.4f;
		y += min_ty * dy + ny * 0.4f;

		if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;
	}
}

void CBoomerangBro::Render() {
	int ani = -1;
	switch (state) {
	case BBRO_STATE_IDLE: {
		if (nx > 0)
			ani = BBRO_ANI_IDLE_RIGHT;
		else
			ani = BBRO_ANI_IDLE_LEFT;
		break;
	}
	case BBRO_STATE_DEATH: {
		if (nx > 0)
			ani = BBRO_ANI_DEATH_RIGHT;
		else
			ani = BBRO_ANI_DEATH_LEFT;
		break;
	}
	default: {
		if ((Ready) && (ammo > 0)) {
			if (nx > 0)
				ani = BBRO_ANI_WALK_ARMED_RIGHT;
			else
				ani = BBRO_ANI_WALK_ARMED_LEFT;
		}
		else {
			if (nx > 0)
				ani = BBRO_ANI_WALK_UNARMED_RIGHT;
			else
				ani = BBRO_ANI_WALK_UNARMED_LEFT;
		}
		break;
	}
	}

	if (ani == BBRO_ANI_WALK_ARMED_LEFT) {
		animation_set->at(ani)->Render(x, y - 5);
	}
	else {
		if (ani == BBRO_ANI_WALK_ARMED_RIGHT)
			animation_set->at(ani)->Render(x - 8, y - 5);
		else
			animation_set->at(ani)->Render(x, y);
	}
}

void CBoomerangBro::GetBoundingBox(float& l, float& t, float& r, float& b) {
	if (state != BBRO_STATE_DEATH) {
		l = x;
		t = y;
		r = x + BBRO_BBOX_WIDTH;
		b = y + BBRO_BBOX_HEIGHT;
	}
}