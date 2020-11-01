#pragma once
#include "GameObject.h"

#define GOOMBA_WALKING_SPEED 0.05f;
#define GOOMBA_DIE_DEFLECT_SPEED 0.5f
#define GOOMBA_GRAVITY 0.002f

#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT_DEAD 9

#define GOOMBA_STATE_WALK 100
#define GOOMBA_STATE_FLATTENED 200
#define GOOMBA_STATE_DEATH 250

#define GOOMBA_ANI_WALKING 0
#define GOOMBA_ANI_DEATH 1

class CGoomba : public CGameObject
{
protected:
	int min_x;
	int max_x;
public:
	CGoomba(int x, int X);
	virtual void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);
	virtual void SetState(int state);

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();
};