#pragma once
#include "GameObject.h"
#include "Mario.h"

#define POWERUP_STATE_PENDING 0
#define POWERUP_STATE_EMERGING 100
#define POWERUP_STATE_NORMAL 200
#define POWERUP_STATE_FLUTTER_LEFT 300
#define POWERUP_STATE_FLUTTER_RIGHT 350

#define POWERUP_TYPE_UNKNOWN 0
#define POWERUP_TYPE_MUSHROOM 1
#define POWERUP_TYPE_LEAF 2
#define POWERUP_TYPE_1UP 3

#define POWERUP_ANI_MUSHROOM 0
#define POWERUP_ANI_LEAF 1
#define POWERUP_ANI_1UP 2

#define POWERUP_BBOX_WIDTH 16
#define POWERUP_BBOX_HEIGHT_MUSHROOM 16
#define POWERUP_BBOX_HEIGHT_LEAF 14
#define POWERUP_BBOX_PENDING_WIDTH 6

#define MUSHROOM_EMERGE_SPEED 0.02f
#define LEAF_EMERGE_SPEED 0.5f
#define MUSHROOM_SPEED 0.075f
#define MUSHROOM_GRAVITY 0.002f
#define LEAF_SPEED 0.07f
#define LEAF_GRAVITY 0.05f
#define LEAF_RESISTANCE 0.000075f

#define DELAY_TIME 1000

class CPowerUp :public CGameObject {
	int power;
	bool OneUp;
public:
	CPowerUp(bool IsOneUp);

	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);

	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	void Render();
	
	int GetPower() { return this->power; }
	void SetPower(int power) { this->power = power; }

	void SetState(int state);

	void GetBoundingBox(float& l, float& t, float& r, float& b);
};