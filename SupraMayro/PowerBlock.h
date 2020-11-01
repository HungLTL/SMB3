#pragma once
#include "GameObject.h"
#include "Block.h"

#define PBLOCK_ANI_UNUSED 0
#define PBLOCK_ANI_USED 1

class CPBlock :public CBlock {
	bool IsUsed;
public:
	CPBlock() :CBlock() {
		IsUsed = false;
	}
	void Render();
	void SetState(bool state) { this->IsUsed = state; }
	bool GetState() { return this->IsUsed; }
};