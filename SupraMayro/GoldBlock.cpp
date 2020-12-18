#include "GoldBlock.h"
#include "Game.h"

void CGoldBlock::Render() {
	if (active)
		animation_set->at(0)->Render(x, y);
}

void CGoldBlock::ChangeState() {
	if (active)
		active = false;
	CGame::GetInstance()->AddScore(10);
}

void CGoldBlock::GetBoundingBox(float& l, float& t, float& r, float& b) {
	if (active) {
		l = x;
		t = y;
		r = x + BLOCK_BBOX_WIDTH;
		b = y + BLOCK_BBOX_WIDTH;
	}
}