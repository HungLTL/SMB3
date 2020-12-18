#include "PowerBlock.h"
#include "Game.h"

void CPBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_object) {
	CGameObject::Update(dt);

	vy += PBLOCK_RETRACT_SPEED;

	y += dy;
	
	if (y >= y0) {
		vy = 0;
		y = y0;
	}
}

void CPBlock::Render() {
	int ani = 0;
	if (!IsUsed)
		ani = PBLOCK_ANI_UNUSED;
	else
		ani = PBLOCK_ANI_USED;
	animation_set->at(ani)->Render(x, y);
	//RenderBoundingBox();
}