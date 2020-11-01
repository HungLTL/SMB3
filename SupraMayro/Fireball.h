#pragma once
#include "Block.h"

#include "GameObject.h"
#include "Mario.h"
#include "BackgroundPlatform.h"

#define FIREBALL_BBOX_WIDTH 8

#define FIREBALL_LEFT 0
#define FIREBALL_RIGHT 1

#define FIREBALL_SPEED 0.25f
#define FIREBALL_GRAVITY 0.002f
#define FIREBALL_BOUNCE_HEIGHT 0.25f

class CFireball :public CBlock {
	bool Direction;
	int Bounces;
	bool render;
public:
	CFireball(bool dir) { this->Direction = dir; Bounces = 0; render = true; }
	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);

	int GetBounces(){ return Bounces; }

	void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	void Render();

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);

	~CFireball();
};