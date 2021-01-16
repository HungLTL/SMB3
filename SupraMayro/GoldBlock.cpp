#include "GoldBlock.h"

void CGoldBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_object) {
	CGameObject::Update(dt);

	if (IsNudging)
		vy += BLOCK_RETRACT_SPEED;

	y += dy;

	if (y >= y0) {
		if (IsNudging)
			IsNudging = false;
		y = y0;
		vy = 0;
	}
}

void CGoldBlock::SetStatus() {
	if (!IsNudging) {
		IsNudging = true;
		vy = -BLOCK_SHIFT_SPEED;
	}
}