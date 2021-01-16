#pragma once
#include "Block.h"

#define BLOCK_SHIFT_SPEED 0.1f
#define BLOCK_RETRACT_SPEED 0.02f

class CGoldBlock :public CBlock {
protected:
	bool IsNudging;
public:
	CGoldBlock() :CBlock() { IsNudging = false; }

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);

	virtual void SetStatus();

	bool GetNudge() { return IsNudging; }
};