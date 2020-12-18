#pragma once
#include "Block.h"

class CGoldBlock :public CBlock {
protected:
public:
	CGoldBlock() :CBlock() { this->active = true; }
	void Render();
	void GetBoundingBox(float& l, float& t, float& r, float& b);

	void ChangeState();
	bool GetState() { return this->active; }
};