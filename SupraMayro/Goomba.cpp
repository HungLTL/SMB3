#include <algorithm>
#include "Goomba.h"
#include "BackgroundPlatform.h"
#include "Mario.h"
#include "Koopa.h"

CGoomba::CGoomba(int x, int X)
{
	SetState(GOOMBA_STATE_WALK);
	this->min_x = x;
	this->max_x = X;
}

void CGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == GOOMBA_STATE_WALK) {
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
			if (dynamic_cast<CGoomba*>(e->obj) || dynamic_cast<CKoopa*>(e->obj)) {
				delete e;
				continue;
			}

			if ((dynamic_cast<CBackgroundPlatform*>(e->obj)) || (dynamic_cast<CMario*>(e->obj))) {
				if (dynamic_cast<CBackgroundPlatform*>(e->obj)) {
					if (e->ny < 0)
						coEvents.push_back(e);
				}
				if (dynamic_cast<CMario*>(e->obj)) {
					if (e->ny != 0)
						coEvents.push_back(e);
				}
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
	CGameObject::Update(dt); 

	if (state != GOOMBA_STATE_FLATTENED)
		vy += GOOMBA_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0) {
		x += dx;
		y += dy;

		if (vx < 0 && x < min_x) {
			x = min_x; vx = -vx;
		}

		if (vx > 0 && x > max_x) {
			x = max_x; vx = -vx;
		}
	}
	else {
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++) {
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (dynamic_cast<CGameObject*>(e->obj)) {
				if (e->ny < 0) {
					vy = 0;
				}
				if (e->nx != 0) {
					vx = -vx;
				}
			}
		}
	}

	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CGoomba::Render()
{
	int ani = GOOMBA_ANI_WALKING;
	if ((state == GOOMBA_STATE_DEATH)||(state == GOOMBA_STATE_FLATTENED)) {
		ani = GOOMBA_ANI_DEATH;
	}
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
		break;
	}
	case GOOMBA_STATE_DEATH:
		vx = 0;
		vy = -GOOMBA_DIE_DEFLECT_SPEED;
		break;
	case GOOMBA_STATE_WALK:
		vx = -GOOMBA_WALKING_SPEED;
	}
}