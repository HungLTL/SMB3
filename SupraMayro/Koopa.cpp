#include <algorithm>
#include "Koopa.h"
#include "BackgroundPlatform.h"
#include "Mario.h"
#include "PowerBlock.h"

void CKoopa::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents) {
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f) {
			if (dynamic_cast<CGoomba*>(e->obj) || dynamic_cast<CKoopa*>(e->obj)) {
				delete e;
				continue;
			}

			if ((dynamic_cast<CBackgroundPlatform*>(e->obj))) {
				if (dynamic_cast<CBackgroundPlatform*>(e->obj)) {
					if (e->ny < 0)
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

void CKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	if (state != KOOPA_STATE_DORMANT)
		vy += KOOPA_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0) {
		x += dx;
		y += dy;

		if ((state == KOOPA_STATE_WALK_LEFT) || (state == KOOPA_STATE_WALK_RIGHT)) {
			if (vx < 0 && x < min_x) {
				x = min_x;
				vx = -vx;
				state = KOOPA_STATE_WALK_RIGHT;
			}

			if (vx > 0 && x > max_x) {
				x = max_x;
				vx = -vx;
				state = KOOPA_STATE_WALK_LEFT;
			}
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
					switch (state) {
					case KOOPA_STATE_WALK_LEFT:
						state = KOOPA_STATE_WALK_RIGHT;
						break;
					case KOOPA_STATE_WALK_RIGHT:
						state = KOOPA_STATE_WALK_LEFT;
						break;
					case KOOPA_STATE_PINBALL_RIGHT:
						state = KOOPA_STATE_PINBALL_LEFT;
						break;
					case KOOPA_STATE_PINBALL_LEFT:
						state = KOOPA_STATE_PINBALL_LEFT;
						break;
					}
					if (dynamic_cast<CPBlock*>(e->obj)) {
						CPBlock* PBlock = dynamic_cast<CPBlock*>(e->obj);
						if (!PBlock->GetState())
							PBlock->SetState(true);
					}
				}
			}
		}
	}

	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CKoopa::Render() {
	int ani = KOOPA_ANI_WALK_LEFT;
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
	animation_set->at(ani)->Render(x, y);
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
		break;
	case KOOPA_STATE_PINBALL_LEFT:
		nx = -1;
		vx = -KOOPA_PINBALL_SPEED;
		break;
	case KOOPA_STATE_PINBALL_RIGHT:
		nx = 1;
		vx = KOOPA_PINBALL_SPEED;
		break;
	case KOOPA_STATE_DORMANT:
		vx = 0;
		vy = 0;
		y += KOOPA_BBOX_HEIGHT - KOOPA_SHELL_BBOX_HEIGHT + 1;
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