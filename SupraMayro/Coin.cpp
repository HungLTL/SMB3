#include <algorithm>
#include "Coin.h"
#include "PowerBlock.h"
#include "Mario.h"
#include "Koopa.h"

#include "Game.h"

CCoin::CCoin(int type) {
	this->active = true;
	this->TransformedFromBlock = false;
	if (type == 0)
		SetState(COIN_STATE_STATIC);
	else {
		SetState(COIN_STATE_DORMANT);
	}
}

void CCoin::SetState(int state) {
	CGameObject::SetState(state);

	switch (state) {
	case COIN_STATE_DORMANT:
	case COIN_STATE_STATIC:
		vy = 0;
		break;
	case COIN_STATE_ACTIVE:
		vy -= COIN_BOUNCE;
		break;
	}
}

void CCoin::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents) {
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f) {
			if (dynamic_cast<CMario*>(e->obj)) {
				if (state == COIN_STATE_STATIC)
					coEvents.push_back(e);
			}
			else {
				if (dynamic_cast<CPBlock*>(e->obj)) {
					if (state == COIN_STATE_DORMANT)
						coEvents.push_back(e);
					else
						delete e;
				}
				else
					delete e;
			}
		}
		else
			delete e;
	}
	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void CCoin::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	if (state == COIN_STATE_ACTIVE) {
		vy += COIN_GRAVITY;
		if (y > y0)
			this->active = false;
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0)
		y += dy;
	else {
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++) {
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (dynamic_cast<CMario*>(e->obj)) {
				this->active = false;
				CGame::GetInstance()->AddCoin();
				CGame::GetInstance()->AddScore(50);
			}

			if (dynamic_cast<CPBlock*>(e->obj)) {
				if (state == COIN_STATE_DORMANT) {
					CPBlock* block = dynamic_cast<CPBlock*>(e->obj);
					float fx, fy;
					block->GetPosition(fx, fy);
					this->SetPosition(fx, fy);
					this->y0 = fy;
					this->SetState(COIN_STATE_ACTIVE);
					CGame::GetInstance()->AddCoin();
					CGame::GetInstance()->AddScore(100);
				}
			}
		}
	}
}

void CCoin::Render() {
	int ani;
	switch (state) {
	case COIN_STATE_ACTIVE:
		ani = COIN_ANI_ACTIVE;
		break;
	case COIN_STATE_STATIC:
		ani = COIN_ANI_STATIC;
		break;
	default:
		return;
	}
	if (active)
		animation_set->at(ani)->Render(x, y);
	//RenderBoundingBox();
}

void CCoin::GetBoundingBox(float& l, float& t, float& r, float& b) {
	if (active) {
		if (state == COIN_STATE_STATIC) {
			l = x + 1;
			t = y + 1;
			r = l + COIN_BBOX_WIDTH;
			b = t + COIN_BBOX_HEIGHT;
		}
		else {
			if (state == COIN_STATE_DORMANT) {
				l = x + 5;
				t = y;
				r = l + COIN_BBOX_DORMANT_WIDTH;
				b = y + COIN_BBOX_HEIGHT;
			}
		}
	}
}