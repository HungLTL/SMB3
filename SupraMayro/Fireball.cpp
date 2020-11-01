#include <algorithm>
#include "Fireball.h"
#include "Goomba.h"
#include "Koopa.h"

void CFireball::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents) {
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f) {
			if (dynamic_cast<CMario*>(e->obj) || dynamic_cast<CFireball*>(e->obj)) {
				delete e;
				continue;
			}

			if (dynamic_cast<CBackgroundPlatform*>(e->obj)) {
				if (e->ny < 0)
					coEvents.push_back(e);
			}
			else {
				coEvents.push_back(e);
			}
		}
		else
			delete e;
	}
	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void CFireball::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	vy += FIREBALL_GRAVITY * dt;
	if (Direction)
		vx = FIREBALL_SPEED;
	else
		vx = -FIREBALL_SPEED;

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

		for (UINT i = 0; i < coEventsResult.size(); i++) {
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (dynamic_cast<CGameObject*>(e->obj)) {
				if (e->ny < 0) {
					vy = -FIREBALL_BOUNCE_HEIGHT;
					Bounces++;
					if (Bounces == 3)
						render = false;
				}
				if (e->nx != 0)
					render = false;
			}
			if (dynamic_cast<CGoomba*>(e->obj)) {
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
				if (goomba->GetState() == GOOMBA_STATE_WALK) {
					goomba->SetState(GOOMBA_STATE_DEATH);
					render = false;
				}
			}
			if (dynamic_cast<CKoopa*>(e->obj)) {
				CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
				if ((koopa->GetState() != KOOPA_STATE_DEATH)) {
					koopa->SetState(KOOPA_STATE_DEATH);
					render = false;
				}
			}
		}
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CFireball::Render() {
	int ani;
	if (Direction)
		ani = FIREBALL_RIGHT;
	else
		ani = FIREBALL_LEFT;
	if (render)
		animation_set->at(ani)->Render(x, y);
	//RenderBoundingBox();
}

void CFireball::GetBoundingBox(float& left, float& top, float& right, float& bottom) {
	left = x;
	top = y;
	right = x + FIREBALL_BBOX_WIDTH;
	bottom = y + FIREBALL_BBOX_WIDTH;
}

CFireball::~CFireball() {
}