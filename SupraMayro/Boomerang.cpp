#include "Boomerang.h"
#include "BoomerangBro.h"

#include "Game.h"
#include "PlayScene.h"

CBoomerang::CBoomerang(bool direction) {
	this->Direction = direction;
	if (direction)
		vx = BOOMERANG_SPEED;
	else
		vx = -BOOMERANG_SPEED;
	vy = -BOOMERANG_INIT_HEIGHT;

	max_x = min_x = max_y = NULL;

	this->SetAnimationSet((CAnimationSets::GetInstance())->Get(24));
}

void CBoomerang::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents) {
	for (UINT i = 0; i < coObjects->size(); i++) {
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f) {
			if (dynamic_cast<CBoomerangBro*>(e->obj))
				coEvents.push_back(e);
			else
				delete e;
		}
		else
			delete e;
	}
}

void CBoomerang::SetBoundaries() {
	if (Direction)
		max_x = (int)x0 + MAX_DISTANCE;
	else
		min_x = (int)x0 - MAX_DISTANCE;

	max_y = (int)y0 + 13;
}

void CBoomerang::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	vy += BOOMERANG_GRAVITY;

	float cx, cy;
	int scrw, scrh;
	CGame::GetInstance()->GetCamPos(cx, cy);
	scrh = CGame::GetInstance()->GetScreenHeight();
	scrw = CGame::GetInstance()->GetScreenWidth();

	if ((x < cx - WIDTH) || (x > cx + scrw) || (y < cy - WIDTH) || (y > cy + scrw))
		dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(this);

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0) {
		x += dx;
		y += dy;

		if (!Direction) {
			if (x <= min_x) {
				x = min_x;
				vx = -vx;
			}
		}
		else {
			if (x >= max_x) {
				x = max_x;
				vx = -vx;
			}
		}

		if (y >= max_y) {
			y = max_y;
			vy = 0;
		}
	}
	else {
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++) {
			LPCOLLISIONEVENT e = coEventsResult[i];

			dynamic_cast<CBoomerangBro*>(e->obj)->AddAmmo();
		}
		dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(this);
	}
}

void CBoomerang::Render() {
	if (Direction)
		animation_set->at(BMRANG_ANI_RIGHT)->Render(x, y);
	else
		animation_set->at(BMRANG_ANI_LEFT)->Render(x, y);
}