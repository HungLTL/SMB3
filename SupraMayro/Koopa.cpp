#include <algorithm>
#include "Koopa.h"
#include "BackgroundPlatform.h"
#include "Mario.h"
#include "PowerBlock.h"
#include "GoldBlock.h"
#include "Coin.h"
#include "PowerUp.h"
#include "Fireball.h"

#include "Game.h"
#include "PlayScene.h"

CKoopa::CKoopa(int x, int X) {
	this->min_x = x;
	this->max_x = X;
	this->playerX = this->playerY = NULL;

	this->Para = false;
	this->IsBeingCarried = false;
	
	this->dormant = 0;
	SetState(KOOPA_STATE_WALK_LEFT);
}

CKoopa::CKoopa(int x, int X, int px, int py) {
	this->min_x = x;
	this->max_x = X;
	this->playerX = px;
	this->playerY = py;

	this->Para = true;

	this->dormant = 0;
	SetState(KOOPA_STATE_WALK_LEFT);
}

void CKoopa::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents) {
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f) {
			if ((dynamic_cast<CMario*>(e->obj))
				|| (dynamic_cast<CCoin*>(e->obj))
				|| (dynamic_cast<CFireball*>(e->obj))
				|| (dynamic_cast<CTail*>(e->obj))
				|| (dynamic_cast<CPowerUp*>(e->obj))) {
				delete e;
				continue;
			}

			if (dynamic_cast<CGoomba*>(e->obj)) {
				if ((state == KOOPA_STATE_PINBALL_LEFT) || (state == KOOPA_STATE_PINBALL_RIGHT)) {
					delete e;
					continue;
				}
			}

			if (dynamic_cast<CKoopa*>(e->obj)) {
				if ((Para) || (dynamic_cast<CKoopa*>(e->obj)->GetPara())) {
					delete e;
					continue;
				}
			}

			if ((dynamic_cast<CBackgroundPlatform*>(e->obj))) {
				if (e->ny != 0)
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

void CKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CMario* mario = (dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->getPlayer());
	float fx, fy;
	mario->GetPosition(fx, fy);

	CGameObject::Update(dt);

	if (y > 256.0f)
		dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(this);

	if (state != KOOPA_STATE_DORMANT)
		vy += KOOPA_GRAVITY * dt;
	else {
		if (GetTickCount() - dormant_start > DORMANT_TIME) {
			dormant_start = 0;
			dormant = 0;
			if (nx > 0)
				this->SetState(KOOPA_STATE_WALK_RIGHT);
			else
				this->SetState(KOOPA_STATE_WALK_LEFT);
			this->ModY(KOOPA_SHELL_BBOX_HEIGHT - KOOPA_BBOX_HEIGHT - 1);
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0) {
		if (IsBeingCarried) {
			if (mario->GetCarryStatus()) {
				if (mario->GetNX() > 0) {
					if (mario->GetPCForm() != MARIO_FORM_NORMAL)
						x = fx + MARIO_SUPER_BBOX_WIDTH;
					else
						x = fx + MARIO_NORMAL_BBOX_WIDTH;
				}
				else
					x = fx - KOOPA_BBOX_WIDTH;
				y = fy - 8;
			}
			else {
				this->IsBeingCarried = false;
				if (mario->GetNX() > 0)
					this->SetState(KOOPA_STATE_PINBALL_RIGHT);
				else
					this->SetState(KOOPA_STATE_PINBALL_LEFT);
			}
		}

		x += dx;
		y += dy;


		if ((state == KOOPA_STATE_WALK_LEFT) || (state == KOOPA_STATE_WALK_RIGHT)) {
			if (state == KOOPA_STATE_WALK_LEFT && x < min_x) {
				x = min_x;
				this->SetState(KOOPA_STATE_WALK_RIGHT);
			}

			if (state == KOOPA_STATE_WALK_RIGHT && x > max_x) {
				x = max_x;
				this->SetState(KOOPA_STATE_WALK_LEFT);
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

		if (ny != 0) vy = 0;

		for (UINT i = 0; i < coEventsResult.size(); i++) {
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGameObject*>(e->obj)) {
				if (dynamic_cast<CKoopa*>(e->obj)) {
					CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
					if ((this->state == KOOPA_STATE_PINBALL_LEFT) || (this->state == KOOPA_STATE_PINBALL_RIGHT)) {
						koopa->SetState(KOOPA_STATE_DEATH);
						this->SetState(KOOPA_STATE_DEATH);
					}
				}

				if (e->ny < 0) {
					if (!Para) {
						if (dynamic_cast<CWoodPlatform*>(e->obj)) {
							CWoodPlatform* platform = dynamic_cast<CWoodPlatform*>(e->obj);
							this->min_x = platform->GetX();
							this->max_x = platform->GetX() + platform->GetLength() - WIDTH;
						}
						if (dynamic_cast<CBackgroundPlatform*>(e->obj)) {
							CBackgroundPlatform* platform = dynamic_cast<CBackgroundPlatform*>(e->obj);
							this->min_x = platform->GetX();
							this->max_x = platform->GetX() + platform->GetLength() - WIDTH;
						}
					}
					else
						vy = -0.45;
				}

				if (e->nx != 0) {
					vx = -vx;
					switch (state) {
					case KOOPA_STATE_WALK_LEFT:
						this->SetState(KOOPA_STATE_WALK_RIGHT);
						break;
					case KOOPA_STATE_WALK_RIGHT:
						this->SetState(KOOPA_STATE_WALK_LEFT);
						break;
					case KOOPA_STATE_PINBALL_RIGHT:
						this->SetState(KOOPA_STATE_PINBALL_LEFT);
						break;
					case KOOPA_STATE_PINBALL_LEFT:
						this->SetState(KOOPA_STATE_PINBALL_RIGHT);
						break;
					}
					if (dynamic_cast<CPBlock*>(e->obj)) {
						if ((state == KOOPA_STATE_PINBALL_LEFT) || (state == KOOPA_STATE_PINBALL_RIGHT)) {
							CPBlock* PBlock = dynamic_cast<CPBlock*>(e->obj);
							if (!PBlock->GetState()) {
								PBlock->SetState(true);
								PBlock->ShiftY();
							}
						}
					}
					if (dynamic_cast<CGoldBlock*>(e->obj)) {
						if ((state == KOOPA_STATE_PINBALL_LEFT) || (state == KOOPA_STATE_PINBALL_RIGHT)) {
							if (e->nx != 0)
								dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(e->obj);
						}
					}
				}
			}
		}
	}

	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CKoopa::Render() {
	int ani = -1;

	if (Para) {
		if (state == KOOPA_STATE_WALK_RIGHT)
			ani = PARATROOPA_ANI_RIGHT;
		else
			ani = PARATROOPA_ANI_LEFT;
		
	}
	else {
		switch (state) {
		case KOOPA_STATE_WALK_LEFT:
			ani = KOOPA_ANI_WALK_LEFT;
			break;
		case KOOPA_STATE_WALK_RIGHT:
			ani = KOOPA_ANI_WALK_RIGHT;
			break;
		case KOOPA_STATE_DORMANT:
			ani = KOOPA_ANI_DORMANT;
			break;
		case KOOPA_STATE_PINBALL_LEFT:
		case KOOPA_STATE_PINBALL_RIGHT:
			ani = KOOPA_ANI_PINBALL;
			break;
		case KOOPA_STATE_DEATH:
			ani = KOOPA_ANI_DEATH;
			break;
		}
	}
	
	animation_set->at(ani)->Render(x, y);
	//RenderBoundingBox();
}

void CKoopa::SetState(int state) {
	CGameObject::SetState(state);
	switch (state) {
	case KOOPA_STATE_WALK_LEFT:
		nx = -1;
		vx = -KOOPA_WALKING_SPEED;
		break;
	case KOOPA_STATE_WALK_RIGHT:
		nx = 1;
		vx = KOOPA_WALKING_SPEED;
		break;
	case KOOPA_STATE_DEATH:
		vx = 0;
		vy = -KOOPA_DIE_DEFLECT_SPEED;
		CGame::GetInstance()->AddScore(100);
		break;
	case KOOPA_STATE_PINBALL_LEFT:
		nx = -1;
		dormant_start = 0;
		vx = -KOOPA_PINBALL_SPEED;
		break;
	case KOOPA_STATE_PINBALL_RIGHT:
		nx = 1;
		dormant_start = 0;
		vx = KOOPA_PINBALL_SPEED;
		break;
	case KOOPA_STATE_DORMANT:
		vx = 0;
		vy = 0;
		StartDormant();
		CGame::GetInstance()->AddScore(100);
		break;
	}
}

void CKoopa::GetBoundingBox(float& left, float& top, float& right, float& bottom) {
	if (state != KOOPA_STATE_DEATH) {
		left = x;
		top = y;
		right = x + KOOPA_BBOX_WIDTH;
		if ((state == KOOPA_STATE_WALK_LEFT) || (state == KOOPA_STATE_WALK_RIGHT))
			bottom = y + KOOPA_BBOX_HEIGHT;
		else
			bottom = y + KOOPA_SHELL_BBOX_HEIGHT;
	}
}