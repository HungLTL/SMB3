#pragma once
#include "GameObject.h"
#include "Goomba.h"

#define KOOPA_WALKING_SPEED 0.05f
#define KOOPA_PINBALL_SPEED 0.3f
#define KOOPA_DIE_DEFLECT_SPEED 0.5f
#define KOOPA_GRAVITY 0.002f

#define KOOPA_BBOX_WIDTH 16
#define KOOPA_BBOX_HEIGHT 26
#define KOOPA_SHELL_BBOX_HEIGHT 16

#define KOOPA_STATE_WALK_LEFT 100
#define KOOPA_STATE_WALK_RIGHT 150
#define KOOPA_STATE_DORMANT 200
#define KOOPA_STATE_PINBALL_LEFT 300
#define KOOPA_STATE_PINBALL_RIGHT 350
#define KOOPA_STATE_DEATH 400

#define KOOPA_ANI_WALK_LEFT 0
#define KOOPA_ANI_WALK_RIGHT 1
#define KOOPA_ANI_DORMANT 2
#define KOOPA_ANI_PINBALL 3
#define KOOPA_ANI_DEATH 4

class CKoopa :public CGoomba {
public:
	CKoopa(int x, int X) :CGoomba(x, X) { SetState(KOOPA_STATE_WALK_LEFT); }
	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);
	void SetState(int state);

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	void Render();
};