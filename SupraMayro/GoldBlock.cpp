#include "GoldBlock.h"

void CGoldBlock::Render() {
	if (render)
		animation_set->at(0)->Render(x, y);
}

void CGoldBlock::GetBoundingBox(float& l, float& t, float& r, float& b) {
	if (render) {
		l = x;
		t = y;
		r = x + BLOCK_BBOX_WIDTH;
		b = y + BLOCK_BBOX_WIDTH;
	}
}