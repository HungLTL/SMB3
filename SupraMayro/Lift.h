#pragma once
#include "GameObject.h"

#define LIFT_STATE_IDLE 0
#define LIFT_STATE_MOVING 100
#define LIFT_STATE_FALLING 200

#define LIFT_ANI_LEFT 0
#define LIFT_ANI_MID 1
#define LIFT_ANI_RIGHT 2

#define LIFT_GRAVITY 0.00012f
#define LIFT_SPEED 0.035f

class CLift :public CGameObject {
public:
	void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	void Render();

	void SetState(int state);
	void GetBoundingBox(float& l, float& t, float& r, float& b);

	void GetLiftCoordinates(float& x, float& X) { x = this->x; X = this->x + WIDTH * 3; }
};