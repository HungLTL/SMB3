#include <algorithm>
#include "MapMario.h"
#include "MapLevelBlock.h"

#include "Game.h"
#include "MapScene.h"

CMapMario::CMapMario() {
	move_start = 0;

	if (CGame::GetInstance()->GetPrevForm() == NULL)
		form = FORM_NORMAL;
	else
		form = CGame::GetInstance()->GetPrevForm();
}

void CMapMario::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents) {
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f) {
			if (dynamic_cast<CMapLevelBlock*>(e->obj)) {
				delete e;
				continue;
			}
			else
				coEvents.push_back(e);
		}
		else
			delete e;
	}
	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void CMapMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	if (y <= 15) {
		y = 15;
		vy = 0;
	}

	if (y >= 137) {
		y = 137;
		vy = 0;
	}

	if (vx != 0 || vy != 0) {
		if (GetTickCount() - move_start >= MOVE_TIME) {
			move_start = 0;
			vx = vy = 0;
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0)
	{
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

		if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;
	}
}

void CMapMario::Render() {
	int ani = -1;
	switch (form) {
	case FORM_NORMAL:
		ani = ANI_NORMAL;
		break;
	case FORM_SUPER:
		ani = ANI_SUPER;
		break;
	case FORM_RACCOON:
		ani = ANI_RACCOON;
		break;
	case FORM_FIRE:
		ani = ANI_FIRE;
		break;
	}

	if (form == FORM_NORMAL)
		animation_set->at(ani)->Render(x, y);
	else
		animation_set->at(ani)->Render(x, y - 6);
}

void CMapMario::SetState(int state) {
	CGameObject::SetState(state);
	
	StartMove();
	switch (state) {
	case STATE_MOVE_UP:
		vy = -MOVESPEED_VERTICAL;
		break;
	case STATE_MOVE_LEFT:
		vx = -MOVESPEED_HORIZONTAL;
		break;
	case STATE_MOVE_DOWN:
		vy = MOVESPEED_VERTICAL;
		break;
	case STATE_MOVE_RIGHT:
		vx = MOVESPEED_HORIZONTAL;
		break;
	}
}

void CMapMario::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = x;
	t = y;
	r = x + BBOX_WIDTH;
	b = y + BBOX_HEIGHT;
}