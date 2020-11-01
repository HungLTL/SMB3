#include "PowerBlock.h"
#include "Mario.h"
#include "Game.h"

void CPBlock::Render() {
	int ani = 0;
	if (!IsUsed)
		ani = PBLOCK_ANI_UNUSED;
	else
		ani = PBLOCK_ANI_USED;
	animation_set->at(ani)->Render(x, y);
}