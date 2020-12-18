#pragma once
#include "GameObject.h"
#include "Block.h"

#define PBLOCK_ANI_UNUSED 0
#define PBLOCK_ANI_USED 1
#define PBLOCK_SHIFT_SPEED 0.1f
#define PBLOCK_RETRACT_SPEED 0.02f

#define COIN 0
#define POWERUP 1
#define MUSHROOM_1UP 2

class CPBlock :public CBlock {
	bool IsUsed;
public:
	CPBlock(int type) :CBlock() {
		IsUsed = false;
	}

	void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	void Render();

	void SetState(bool state) { this->IsUsed = state; }
	bool GetState() { return this->IsUsed; }

	void ShiftY() { vy -= PBLOCK_SHIFT_SPEED; }
};