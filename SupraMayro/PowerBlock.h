#pragma once
#include "GameObject.h"
#include "GoldBlock.h"

#define PBLOCK_ANI_UNUSED 0
#define PBLOCK_ANI_USED 1

#define COIN 0
#define POWERUP 1
#define MUSHROOM_1UP 2
#define FLOWER 3

class CPBlock :public CGoldBlock {
	int type;
	bool ActivatedAtRightSide;
protected:
	bool IsUsed;
public:
	CPBlock() :CGoldBlock() { type = NULL; ActivatedAtRightSide = true; IsUsed = false; }
	CPBlock(int type) :CGoldBlock() {
		IsUsed = false;
		ActivatedAtRightSide = true;
		this->type = type;
	}

	void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	void Render();

	void SetState(bool state) { this->IsUsed = state; }
	bool GetState() { return this->IsUsed; }
	void SetStatus();
	void ChangeActivationSide() { ActivatedAtRightSide = false; }
};