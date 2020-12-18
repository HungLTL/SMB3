#pragma once
#include "Block.h"

class CMapLevelBlock :public CBlock {
	int scene_id;
public:
	CMapLevelBlock();
	CMapLevelBlock(int id);

	void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);

	void GetBoundingBox(float& l, float& t, float& r, float& b);
};