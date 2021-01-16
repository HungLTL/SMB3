#include <algorithm>
#include "Koopa.h"
#include "BackgroundPlatform.h"
#include "Mario.h"
#include "PowerBlock.h"
#include "GoldBlock.h"
#include "CoinBlock.h"
#include "Coin.h"
#include "PowerUp.h"
#include "Fireball.h"
#include "Goomba.h"
#include "PiranhaPlant.h"

#include "Game.h"
#include "PlayScene.h"

CKoopa::CKoopa(int x, int X) {
	this->min_x = x;
	this->max_x = X;
	this->playerX = this->playerx = this->min_y = this->max_y = NULL;

	this->Para = this->IsBeingCarried = this->active = this->IsStationary = this->IsFlyingUp = false;
	
	this->dormant = 0;
	this->dormant_start = 0;
	SetState(KOOPA_STATE_WALK_LEFT);
}

CKoopa::CKoopa(int x, int X, int px, int pX) {
	this->min_x = x;
	this->max_x = X;
	this->playerx = px;
	this->playerX = pX;
	this->min_y = this->max_y = NULL;

	this->Para = true;
	this->IsBeingCarried = this->active = this->IsStationary = this->IsFlyingUp = false;

	this->dormant = 0;
	this->dormant_start = 0;
	SetState(KOOPA_STATE_WALK_LEFT);
}

CKoopa::CKoopa(int x, int y, int Y) {
	this->min_x = this->max_x = x;
	this->playerx = this->playerX = NULL;
	this->min_y = y;
	this->max_y = Y;

	this->Para = this->IsStationary = true;
	this->IsBeingCarried = this->active = this->IsFlyingUp = false;
	vy = KOOPA_FLY_SPEED;

	this->dormant = 0;
	this->dormant_start = 0;
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
			else {
				if (dynamic_cast<CGoldBlock*>(e->obj)) {
					if (dynamic_cast<CPBlock*>(e->obj))
						coEvents.push_back(e);
					else {
						if (e->nx != 0) {
							if ((state == KOOPA_STATE_WALK_LEFT) || (state == KOOPA_STATE_WALK_RIGHT))
								delete e;
							else
								coEvents.push_back(e);
						}
						else
							coEvents.push_back(e);
					}
				}
				else
					coEvents.push_back(e);
			}
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

	if (state != KOOPA_STATE_DORMANT) {
		if (!IsStationary) {
			if (Para)
				vy += PARAKOOPA_GRAVITY * dt;
			else
				vy += KOOPA_GRAVITY * dt;
		}
		else {
			if (!Para)
				vy += KOOPA_GRAVITY * dt;
		}
	}
	else {
		if (!IsBeingCarried)
			vy += KOOPA_GRAVITY * dt;
		if (GetTickCount() - dormant_start > DORMANT_TIME) {
			dormant_start = 0;
			dormant = 0;
			if (nx > 0)
				this->SetState(KOOPA_STATE_WALK_RIGHT);
			else
				this->SetState(KOOPA_STATE_WALK_LEFT);
			IsBeingCarried = false;
			this->ModY(KOOPA_SHELL_BBOX_HEIGHT - KOOPA_BBOX_HEIGHT - 1);
		}
	}

	if (Para) {
		if (!IsStationary) {
			if (!active) {
				if ((fx >= playerx) && (fx <= playerX)) {
					active = true;
					if (nx > 0)
						SetState(KOOPA_STATE_WALK_RIGHT);
					else
						SetState(KOOPA_STATE_WALK_LEFT);
				}
				else {
					if (fx <= x)
						nx = -1;
					else
						nx = 1;
				}
			}
		}
		else {
			if (IsFlyingUp)
				vy += KOOPA_SLOWFALL;
			else
				vy -= KOOPA_SLOWFALL;
		}
	}

	for (size_t i = 0; i < coObjects->size(); i++) {
		if (dynamic_cast<CGoldBlock*>(coObjects->at(i))) {
			CGoldBlock* block = dynamic_cast<CGoldBlock*>(coObjects->at(i));
			float l, t, r, b, bl, bt, br, bb;
			this->GetBoundingBox(l, t, r, b);
			block->GetBoundingBox(bl, bt, br, bb);

			if (l < br && r > bl && t < bb && b > bt) {
				if (block->GetNudge()) {
					SetState(KOOPA_STATE_DORMANT);
					vy = -KOOPA_DIE_DEFLECT_SPEED * 2 / 3;
					vx = KOOPA_BOUNCE_RECOIL;
				}
			}
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

				if (mario->GetPCForm() != MARIO_FORM_NORMAL)
					y = fy;
				else y = fy - 4.0f;
			}
			else {
				this->IsBeingCarried = false;
				if (mario->GetNX() > 0)
					this->SetState(KOOPA_STATE_PINBALL_RIGHT);
				else
					this->SetState(KOOPA_STATE_PINBALL_LEFT);
			}
		}

		if (!Para) {
			x += dx;
			y += dy;
		}
		else {
			if (!IsStationary) {
				if (active) {
					x += dx;
					y += dy;
				}
				else
					vx = vy = 0;
			}
			else
				y += dy;
		}


		if ((state == KOOPA_STATE_WALK_LEFT) || (state == KOOPA_STATE_WALK_RIGHT)) {
			if (state == KOOPA_STATE_WALK_LEFT && x < min_x) {
				x = (float)min_x;
				this->SetState(KOOPA_STATE_WALK_RIGHT);
			}

			if (state == KOOPA_STATE_WALK_RIGHT && x > max_x) {
				x = (float)max_x;
				this->SetState(KOOPA_STATE_WALK_LEFT);
			}
		}

		if (IsStationary) {
			if (Para) {
				if (y <= min_y) {
					y = (float)min_y;
					IsFlyingUp = false;
					vy = KOOPA_FLY_SPEED;
				}

				if (y >= max_y) {
					y = (float)max_y;
					IsFlyingUp = true;
					vy = -KOOPA_FLY_SPEED;
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

				if (dynamic_cast<CPiranhaPlant*>(e->obj)) {
					dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(e->obj);
					CGame::GetInstance()->AddScore(100);
				}

				if (e->ny < 0) {
					if (!Para) {
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
						if (state == KOOPA_STATE_DORMANT)
							vx = 0;
					}
					else
						vy = -PARATROOPA_BOUNCE_SPEED;
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

					if (dynamic_cast<CGoldBlock*>(e->obj)) {
						if ((state == KOOPA_STATE_PINBALL_LEFT) || (state == KOOPA_STATE_PINBALL_RIGHT)) {
							if (dynamic_cast<CPBlock*>(e->obj)) {
								CPBlock* PBlock = dynamic_cast<CPBlock*>(e->obj);
								if (!PBlock->GetState()) {
									float px, py;
									PBlock->GetPosition(px, py = 0);

									if (x >= px)
										PBlock->ChangeActivationSide();
									PBlock->SetState(true);
									PBlock->SetStatus();
								}
							}
							else {
								if (dynamic_cast<CCoinBlock*>(e->obj))
									dynamic_cast<CCoinBlock*>(e->obj)->SetStatus();
								else {
									dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(e->obj);
									CGame::GetInstance()->AddScore(10);
								}
							}
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
		if (nx > 0)
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
		case KOOPA_STATE_DORMANT: {
			if (GetTickCount() - dormant_start >= DORMANT_WARNING)
				ani = KOOPA_ANI_EMERGING;
			else
				ani = KOOPA_ANI_DORMANT;
			break;
		}
		case KOOPA_STATE_PINBALL_LEFT:
		case KOOPA_STATE_PINBALL_RIGHT:
			ani = KOOPA_ANI_PINBALL;
			break;
		case KOOPA_STATE_DEATH:
			ani = KOOPA_ANI_DEATH;
			break;
		}
	}
	
	if (ani == KOOPA_ANI_EMERGING)
		animation_set->at(ani)->Render(x, y - 1.0f);
	else
		animation_set->at(ani)->Render(x, y);
	//RenderBoundingBox();
}

void CKoopa::SetState(int state) {
	CGameObject::SetState(state);
	switch (state) {
	case KOOPA_STATE_WALK_LEFT:
		nx = -1;
		if (Para)
			vx = -PARAKOOPA_WALKING_SPEED;
		else
			vx = -KOOPA_WALKING_SPEED;
		break;
	case KOOPA_STATE_WALK_RIGHT:
		nx = 1;
		if (Para)
			vx = PARAKOOPA_WALKING_SPEED;
		else
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