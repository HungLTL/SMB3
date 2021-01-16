#include <algorithm>
#include "PowerUp.h"
#include "PowerBlock.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Coin.h"
#include "BackgroundPlatform.h"

#include "Game.h"
#include "PlayScene.h"

CPowerUp::CPowerUp(int power, bool dir) {
	this->power = power;
	this->Direction = dir;
	this->SetState(POWERUP_STATE_EMERGING);
}

void CPowerUp::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents) {
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f) {
			switch (power) {
			case POWERUP_TYPE_1UP:
			case POWERUP_TYPE_MUSHROOM: {
				if ((dynamic_cast<CGoomba*>(e->obj)) || (dynamic_cast<CKoopa*>(e->obj)) || (dynamic_cast<CCoin*>(e->obj))) {
					delete e;
					continue;
				}

				if ((dynamic_cast<CBackgroundPlatform*>(e->obj))) {
					if (e->ny < 0)
						coEvents.push_back(e);
				}
				else
					coEvents.push_back(e);
				break;
			}
			case POWERUP_TYPE_FLOWER:
			case POWERUP_TYPE_LEAF: {
				if (dynamic_cast<CMario*>(e->obj))
					coEvents.push_back(e);
				else {
					delete e;
					continue;
				}
				break;
			}
			}
		}
		else
			delete e;
	}
	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void CPowerUp::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	CMario* player = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->getPlayer();

	if (state == POWERUP_STATE_NORMAL) {
		if (power != POWERUP_TYPE_FLOWER)
			vy += MUSHROOM_GRAVITY * dt;
	}

	if (power == POWERUP_TYPE_LEAF)
		vy -= LEAF_RESISTANCE * dt;

	if (state == POWERUP_STATE_EMERGING) {
		if (power == POWERUP_TYPE_LEAF) {
			if (y < y0 - (WIDTH * 4)) {
				vy = 0;
				this->SetState(POWERUP_STATE_FLUTTER_RIGHT);
			}
		}
		else {
			if (y < y0 - WIDTH) {
				this->SetState(POWERUP_STATE_NORMAL);
				if (power != POWERUP_TYPE_FLOWER) {
					if (!Direction)
						vx = -vx;
				}
				else
					vx = vy = 0;
			}
		}
	}

	if ((power == POWERUP_TYPE_LEAF) && (state != POWERUP_STATE_EMERGING)) {
		if (x >= x0 + WIDTH * 2) {
			vx = vy = 0;
			this->SetState(POWERUP_STATE_FLUTTER_LEFT);
		}
		else {
			if (x <= x0) {
				vx = vy = 0;
				this->SetState(POWERUP_STATE_FLUTTER_RIGHT);
			}
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0) {
		x += dx;
		y += dy;
	}
	else {
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		x += min_tx * dx + nx * 0.4f;
		y += min_ty * dy + ny * 0.4f;

		if (ny != 0) vy = 0;

		for (UINT i = 0; i < coEventsResult.size(); i++) {
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGameObject*>(e->obj)) {
				if (e->nx != 0) {
					vx = -vx;
				}
			}

			if (dynamic_cast<CMario*>(e->obj)) {
				if (power != POWERUP_TYPE_1UP) {
					CMario* mario = dynamic_cast<CMario*>(e->obj);
					if (mario->GetPCForm() == MARIO_FORM_NORMAL) {
						mario->SetPCForm(MARIO_FORM_SUPER);
						mario->ModY(MARIO_NORMAL_BBOX_HEIGHT - MARIO_SUPER_BBOX_HEIGHT - 1);
					}
					else {
						if (power == POWERUP_TYPE_LEAF)
							mario->SetPCForm(MARIO_FORM_RACCOON);
						else {
							if (power == POWERUP_TYPE_FLOWER)
								mario->SetPCForm(MARIO_FORM_FIRE);
						}
					}
				}
				else
					CGame::GetInstance()->AddLife();

				CGame::GetInstance()->AddScore(1000);
				dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(this);
			}
		}
	}
}

void CPowerUp::Render() {
	int ani = -1;
	switch (power) {
	case POWERUP_TYPE_MUSHROOM:
		ani = POWERUP_ANI_MUSHROOM;
		break;
	case POWERUP_TYPE_1UP:
		ani = POWERUP_ANI_1UP;
		break;
	case POWERUP_TYPE_LEAF:
		ani = POWERUP_ANI_LEAF;
		break;
	case POWERUP_TYPE_FLOWER:
		ani = POWERUP_ANI_FLOWER;
		break;
	}

	animation_set->at(ani)->Render(x, y);
	//RenderBoundingBox();
}

void CPowerUp::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = x;
	t = y;
	r = x + POWERUP_BBOX_WIDTH;
	if (power == POWERUP_TYPE_LEAF)
		b = y + POWERUP_BBOX_HEIGHT_LEAF;
	else
		b = y + POWERUP_BBOX_HEIGHT_MUSHROOM;
}

void CPowerUp::SetState(int state) {
	CGameObject::SetState(state);

	switch (state) {
	case POWERUP_STATE_EMERGING:
		if (power != POWERUP_TYPE_LEAF)
			vy -= MUSHROOM_EMERGE_SPEED;
		else
			vy -= LEAF_EMERGE_SPEED;
		break;
	case POWERUP_STATE_NORMAL:
		vx = MUSHROOM_SPEED;
		break;
	case POWERUP_STATE_FLUTTER_RIGHT:
		vx += LEAF_SPEED;
		vy += LEAF_GRAVITY;
		break;
	case POWERUP_STATE_FLUTTER_LEFT:
		vx -= LEAF_SPEED;
		vy += LEAF_GRAVITY;
		break;
	}
}