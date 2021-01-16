#pragma once
#include "GameObject.h"
#include "Block.h"

#define BMRANG_ANI_LEFT 0
#define BMRANG_ANI_RIGHT 1

#define MAX_DISTANCE 160

#define BOOMERANG_SPEED 0.09f
#define BOOMERANG_INIT_HEIGHT 0.0675f
#define BOOMERANG_GRAVITY 0.00125f

class CBoomerang :public CBlock {
	bool Direction;

	int max_x, min_x, max_y;
public:
	CBoomerang(bool direction);

	void SetBoundaries();

	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);

	void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	void Render();
};