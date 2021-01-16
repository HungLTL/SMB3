#include <algorithm>
#include "Goomba.h"
#include "BackgroundPlatform.h"
#include "Mario.h"
#include "Tail.h"
#include "Koopa.h"

#include "Block.h"
#include "GoldBlock.h"
#include "PowerBlock.h"
#include "Fireball.h"

#include "Game.h"
#include "PlayScene.h"

CGoomba::CGoomba(int x, int X, bool IsPara)
{
	this->min_x = x;
	this->max_x = X;

	this->bounces = 0;
	this->walk = 0;

	if (IsPara)
		this->form = GOOMBA_FORM_PARA;
	else
		this->form = GOOMBA_FORM_NORMAL;

	this->SetState(GOOMBA_STATE_WALK_LEFT);
}

void CGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if ((state == GOOMBA_STATE_WALK_LEFT) || (state == GOOMBA_STATE_FLY) || (state == GOOMBA_STATE_WALK_RIGHT)) {
		left = x;
		top = y;
		right = x + GOOMBA_BBOX_WIDTH;
		bottom = y + GOOMBA_BBOX_WIDTH;
	}
}

void CGoomba::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents) {
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f) {
			if ((dynamic_cast<CMario*>(e->obj)) || (dynamic_cast<CFireball*>(e->obj)) || (dynamic_cast<CTail*>(e->obj))) {
				delete e;
				continue;
			}

			if (dynamic_cast<CFireball*>(e->obj)) {
				CFireball* fireball = dynamic_cast<CFireball*>(e->obj);
				if (fireball->GetType() == FIREBALL_MARIO)
					coEvents.push_back(e);
				else
					delete e;
				continue;
			}

			if (dynamic_cast<CGoomba*>(e->obj)) {
				if ((dynamic_cast<CGoomba*>(e->obj)->form == GOOMBA_FORM_PARA) || (this->form == GOOMBA_FORM_PARA)) {
					delete e;
					continue;
				}
			}

			if (dynamic_cast<CKoopa*>(e->obj)) {
				if ((dynamic_cast<CKoopa*>(e->obj))->GetPara()) {
					delete e;
					continue;
				}
			}

			if (dynamic_cast<CBackgroundPlatform*>(e->obj)) {
				if (e->ny < 0)
					coEvents.push_back(e);
			}
			else
				coEvents.push_back(e);
		}
		else
			delete e;
	}
	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CMario* mario = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->getPlayer();
	CGameObject::Update(dt);

	if (state != GOOMBA_STATE_FLATTENED)
		vy += GOOMBA_GRAVITY * dt;

	if (GetTickCount() - walk_start > GOOMBA_WALK_TIME) {
		if ((state != GOOMBA_STATE_FLY) && (this->form == GOOMBA_FORM_PARA)) {
			walk = 0;
			walk_start = 0;
			this->SetState(GOOMBA_STATE_FLY);
		}
	}

	
	if ((form == GOOMBA_FORM_PARA) && (state != GOOMBA_STATE_FLY)) {
		float cx, cy;
		CGame::GetInstance()->GetCamPos(cx, cy);
		float scrnh = (float)CGame::GetInstance()->GetScreenHeight();
		float scrnw = (float)CGame::GetInstance()->GetScreenWidth();

		if ((x > cx) && (x < cx + scrnw) && (y > cy) && (y < cy + scrnh)) {
			if (mario->GetX() > this->x)
				this->SetState(GOOMBA_STATE_WALK_RIGHT);
			else {
				if (mario->GetX() < this->x)
					this->SetState(GOOMBA_STATE_WALK_LEFT);
			}
		}
	}
	

	if (state == GOOMBA_STATE_FLATTENED) {
		if (GetTickCount() - flattened_start > GOOMBA_FLATTENED_TIME) {
			flattened_start = 0;
			dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(this);
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0) {
		x += dx;
		y += dy;

		if (state==GOOMBA_STATE_WALK_LEFT && x < min_x) {
			x = (float)min_x; SetState(GOOMBA_STATE_WALK_RIGHT);
		}

		if (state==GOOMBA_STATE_WALK_RIGHT && x > max_x) {
			x = (float)max_x; SetState(GOOMBA_STATE_WALK_LEFT);
		}
		
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
			if (dynamic_cast<CGoomba*>(e->obj)) {
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
				if (state == GOOMBA_STATE_WALK_LEFT) {
					goomba->SetState(GOOMBA_STATE_WALK_LEFT);
				}
				else {
					if (state == GOOMBA_STATE_WALK_RIGHT) {
						goomba->SetState(GOOMBA_STATE_WALK_RIGHT);
					}
				}
			}

			if (dynamic_cast<CKoopa*>(e->obj)) {
				CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
				if ((koopa->GetState() == KOOPA_STATE_PINBALL_LEFT) || (koopa->GetState() == KOOPA_STATE_PINBALL_RIGHT))
					SetState(GOOMBA_STATE_DEATH);
			}

			if (dynamic_cast<CGameObject*>(e->obj)) {
				if (e->nx != 0) {
					if (state == GOOMBA_STATE_WALK_LEFT)
						SetState(GOOMBA_STATE_WALK_RIGHT);
					else {
						if (state == GOOMBA_STATE_WALK_RIGHT)
							SetState(GOOMBA_STATE_WALK_LEFT);
					}
				}

				if (e->ny < 0) {
					if (form == GOOMBA_FORM_NORMAL) {
						if (dynamic_cast<CWoodPlatform*>(e->obj)) {
							CWoodPlatform* platform = dynamic_cast<CWoodPlatform*>(e->obj);
							this->min_x = (int)platform->GetX();
							this->max_x = (int)platform->GetX() + (int)platform->GetLength() - WIDTH;
						}
						if (dynamic_cast<CBackgroundPlatform*>(e->obj)) {
							CBackgroundPlatform* platform = dynamic_cast<CBackgroundPlatform*>(e->obj);
							this->min_x = (int)platform->GetX();
							this->max_x = (int)platform->GetX() + (int)platform->GetLength() - WIDTH;
						}
					}
					else {
						if (this->state == GOOMBA_STATE_FLY) {
							if (this->bounces == 3) {
								vy = -GOOMBA_SMALLBOUNCE_DEFLECT_SPEED;
								this->bounces = 0;
								SetState(prevState);
							}
							else {
								vy = -GOOMBA_BIGBOUNCE_DEFLECT_SPEED;
								bounces++;
							}
						}
					}
				}
			}
		}
	}

	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CGoomba::Render()
{
	int ani = -1;
	switch (state) {
	case GOOMBA_STATE_WALK_LEFT:
	case GOOMBA_STATE_WALK_RIGHT:
		if (form == GOOMBA_FORM_NORMAL)
			ani = GOOMBA_ANI_WALKING;
		else
			ani = GOOMBA_ANI_PARA_WALKING;
		break;
	case GOOMBA_STATE_FLY:
		ani = GOOMBA_ANI_PARA_FLY;
		break;
	case GOOMBA_STATE_FLATTENED:
	case GOOMBA_STATE_DEATH:
		ani = GOOMBA_ANI_DEATH;
		break;
	}

	if (form == GOOMBA_FORM_PARA) {
		if (state != GOOMBA_STATE_FLY)
			animation_set->at(ani)->Render(x, y - 3);
		else
			animation_set->at(ani)->Render(x, y - 8);
	}
	else
		animation_set->at(ani)->Render(x, y);

	//RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case GOOMBA_STATE_FLATTENED: {
		y += GOOMBA_BBOX_WIDTH - GOOMBA_BBOX_HEIGHT_DEAD + 1;
		vx = 0;
		vy = 0;
		StartFlattened();
		CGame::GetInstance()->AddScore(100);
		break;
	}
	case GOOMBA_STATE_DEATH:
		vx = 0;
		vy = -GOOMBA_DIE_DEFLECT_SPEED;
		CGame::GetInstance()->AddScore(100);
		break;
	case GOOMBA_STATE_WALK_LEFT:
		nx = -1;
		vx = -GOOMBA_WALKING_SPEED;
		if (form == GOOMBA_FORM_PARA) {
			if (walk == 0)
				StartWalk();
		}
		prevState = GOOMBA_STATE_WALK_LEFT;
		break;
	case GOOMBA_STATE_WALK_RIGHT:
		nx = 1;
		vx = GOOMBA_WALKING_SPEED;
		if (form == GOOMBA_FORM_PARA) {
			if (walk == 0)
				StartWalk();
		}
		prevState = GOOMBA_STATE_WALK_RIGHT;
		break;
	}
}