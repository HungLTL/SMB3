#pragma once
#include "GameObject.h"

#define BBRO_STATE_IDLE 0
#define BBRO_STATE_WALK_LEFT 100
#define BBRO_STATE_WALK_RIGHT 150
#define BBRO_STATE_DEATH 200

#define BBRO_ANI_IDLE_LEFT 0
#define BBRO_ANI_IDLE_RIGHT 1
#define BBRO_ANI_WALK_UNARMED_LEFT 2
#define BBRO_ANI_WALK_UNARMED_RIGHT 3
#define BBRO_ANI_WALK_ARMED_LEFT 4
#define BBRO_ANI_WALK_ARMED_RIGHT 5
#define BBRO_ANI_DEATH_LEFT 6
#define BBRO_ANI_DEATH_RIGHT 7

#define IDLE_TIME 1000
#define CHECK_TIME 6000

#define BBRO_SPEED 0.04f
#define BBRO_DIE_DEFLECT_SPEED 0.3f
#define BBRO_GRAVITY 0.0015f

#define BBRO_BBOX_HEIGHT 24
#define BBRO_BBOX_WIDTH 16

class CBoomerangBro :public CGameObject {
	int min_x, max_x, mid_x;
	int ammo;
	int nextState;

	bool Ready;

	int idle;
	DWORD idle_delay;

	DWORD check_start;
public:
	CBoomerangBro(int x, int X);

	void AddAmmo() { ammo++; }

	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);

	void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	void Render();
	void GetBoundingBox(float& l, float& t, float& r, float& b);

	void StartIdle() { idle = 1; idle_delay = GetTickCount(); }
	void StartCheck() { check_start = GetTickCount(); }

	void SetState(int state);
};