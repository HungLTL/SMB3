#pragma once
#include "GameObject.h"
#include "GoldBlock.h"

#define CBLOCK_ANI 0
#define CBLOCK_ANI_EMPTY 1

class CCoinBlock :public CGoldBlock {
	int uses;
public:
	CCoinBlock(int uses) :CGoldBlock() { this->uses = uses; }

	void Render();

	void SetStatus();
};