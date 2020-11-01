#pragma once
#include "GameObject.h"

#define BLOCK_BBOX_WIDTH 16

class CBlock :public CGameObject {
public:
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};