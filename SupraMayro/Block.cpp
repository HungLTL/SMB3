#include "Block.h"

void CBlock::Render() {
	animation_set->at(0)->Render(x, y);
}

void CBlock::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = x;
	t = y;
	r = x + WIDTH;
	b = y + WIDTH;
}