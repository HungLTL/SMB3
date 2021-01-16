#include <algorithm>
#include "Coin.h"
#include "PowerBlock.h"
#include "Mario.h"
#include "Koopa.h"

#include "Game.h"
#include "PlayScene.h"

CCoin::CCoin(int type) {
	this->TransformedFromBlock = false;
	if (type == 0)
		SetState(COIN_STATE_STATIC);
	else {
		SetState(COIN_STATE_ACTIVE);
	}
}

void CCoin::SetState(int state) {
	CGameObject::SetState(state);

	switch (state) {
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
			if (dynamic_cast<CMario*>(e->obj))
				coEvents.push_back(e);
			else
				delete e;
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
			dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(this);
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
				CGame::GetInstance()->AddCoin();
				CGame::GetInstance()->AddScore(50);
				dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(this);
			}
		}
	}
}

void CCoin::Render() {
	int ani = -1;
	if (TransformedFromBlock)
		ani = COIN_ANI_STATIC;
	else
		ani = COIN_ANI_ACTIVE;
	
	animation_set->at(ani)->Render(x, y);
	//RenderBoundingBox();
}

void CCoin::GetBoundingBox(float& l, float& t, float& r, float& b) {
	if (state == COIN_STATE_STATIC) {
		l = x + 1;
		t = y + 1;
		r = l + COIN_BBOX_WIDTH;
		b = t + COIN_BBOX_HEIGHT;
	}
}