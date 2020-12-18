#pragma once
#include "Block.h"
#include "GameObject.h"
#include "Mario.h"
#include "BackgroundPlatform.h"

#define FIREBALL_BBOX_WIDTH 8

#define FIREBALL_MARIO 0
#define FIREBALL_PIRANHA 1

#define FIREBALL_LEFT 0
#define FIREBALL_RIGHT 1

#define FIREBALL_SPEED 0.25f
#define FIREBALL_GRAVITY 0.002f
#define FIREBALL_BOUNCE_HEIGHT 0.25f
#define FIREBALL_PIRANHA_SPEED 0.6f

class CFireball :public CBlock {
	bool Direction;
	int type;

	float x1, y1;
public:
	CFireball(CMario* player, int type);
	CFireball(CGameObject* host, float x, float y, int type, bool dir);

	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);

	void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	void Render();

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);

	int GetType() { return this->type; }

	~CFireball();
};