#include <algorithm>
#include "PowerUp.h"
#include "PowerBlock.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Coin.h"
#include "BackgroundPlatform.h"

#include "Game.h"
#include "PlayScene.h"

CPowerUp::CPowerUp(bool IsOneUp) {
	if (IsOneUp)
		OneUp = true;
	else
		OneUp = false;
	this->power = POWERUP_TYPE_UNKNOWN;
	this->SetState(POWERUP_STATE_PENDING);
}

void CPowerUp::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents) {
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f) {
			switch (power) {
			case POWERUP_TYPE_UNKNOWN: {
				if (dynamic_cast<CPBlock*>(e->obj))
					coEvents.push_back(e);
				else {
					delete e;
					continue;
				}
				break;
			}
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

	if (state == POWERUP_STATE_NORMAL)
		vy += MUSHROOM_GRAVITY * dt;

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
			if (y < y0 - WIDTH)
				this->SetState(POWERUP_STATE_NORMAL);
			if (((player->GetX() < this->x) && (power == POWERUP_TYPE_MUSHROOM)) || ((player->GetX() >= this->x) && (power == POWERUP_TYPE_1UP)))
				vx = -vx;
		}
	}

	if ((power == POWERUP_TYPE_LEAF) && (state != POWERUP_STATE_EMERGING)) {
		if (x >= x0 + WIDTH * 3) {
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

			if ((dynamic_cast<CPBlock*>(e->obj)) && (state == POWERUP_STATE_PENDING)) {
				CPBlock* powerblock = dynamic_cast<CPBlock*>(e->obj);
				if (e->ny > 0) {
					if (power == POWERUP_TYPE_UNKNOWN) {
						if (OneUp)
							this->power = POWERUP_TYPE_1UP;
						else {
							switch (player->GetPCForm()) {
							case MARIO_FORM_NORMAL:
								this->power = POWERUP_TYPE_MUSHROOM;
								break;
							default:
								this->power = POWERUP_TYPE_LEAF;
								break;
							}
						}
					}
					float fx, fy;
					powerblock->GetPosition(fx, fy);
					this->SetPosition(fx, fy);
					this->x0 = fx;
					this->y0 = fy;
					this->SetState(POWERUP_STATE_EMERGING);
				}
			}

			if (dynamic_cast<CMario*>(e->obj)) {
				if (power != POWERUP_TYPE_1UP) {
					CMario* mario = dynamic_cast<CMario*>(e->obj);
					if (mario->GetPCForm() == MARIO_FORM_NORMAL) {
						mario->SetPCForm(MARIO_FORM_SUPER);
						mario->ModY(MARIO_NORMAL_BBOX_HEIGHT - MARIO_SUPER_BBOX_HEIGHT - 1);
					}
					else
						mario->SetPCForm(MARIO_FORM_RACCOON);
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
	int ani;
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
	default:
		return;
	}

	animation_set->at(ani)->Render(x, y);
	//RenderBoundingBox();
}

void CPowerUp::GetBoundingBox(float& l, float& t, float& r, float& b) {
	if (state == POWERUP_STATE_PENDING) {
		l = x + 5;
		t = y;
		r = l + POWERUP_BBOX_PENDING_WIDTH;
		b = y + POWERUP_BBOX_WIDTH;
	}
	else {
		l = x;
		t = y;
		r = x + POWERUP_BBOX_WIDTH;
		if (power == POWERUP_TYPE_LEAF)
			b = y + POWERUP_BBOX_HEIGHT_LEAF;
		else
			b = y + POWERUP_BBOX_HEIGHT_MUSHROOM;
	}
}

void CPowerUp::SetState(int state) {
	CGameObject::SetState(state);

	switch (state) {
	case POWERUP_STATE_PENDING:
		vy = 0;
		vx = 0;
		break;
	case POWERUP_STATE_EMERGING:
		if ((power == POWERUP_TYPE_MUSHROOM) || (power == POWERUP_TYPE_1UP))
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