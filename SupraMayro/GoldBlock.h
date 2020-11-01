#pragma once
#include "Block.h"

class CGoldBlock :public CBlock {
protected:
	bool render;
public:
	CGoldBlock() :CBlock() { render = true; }
	void Render();
	void GetBoundingBox(float& l, float& t, float& r, float& b);

	void ChangeState(){ if (render) render = false; }
};