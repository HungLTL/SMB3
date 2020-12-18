#include "MapLevelBlock.h"
#include "MapMario.h"

#include "Game.h"
#include "MapScene.h"

CMapLevelBlock::CMapLevelBlock() {
	scene_id = NULL;
}

CMapLevelBlock::CMapLevelBlock(int id) {
	scene_id = id;
}

void CMapLevelBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() != 0) {
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++) {
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CMapMario*>(e->obj))
				dynamic_cast<CMapScene*>(CGame::GetInstance()->GetCurrentScene())->SetDestinationScene(scene_id);
		}
	}
}

void CMapLevelBlock::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = x;
	t = y;
	r = x + WIDTH;
	b = y + WIDTH - 1;
}