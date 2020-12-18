#pragma once
#include "GameObject.h"
#include "Mario.h"

#define GOOMBA_WALKING_SPEED 0.03f
#define GOOMBA_DIE_DEFLECT_SPEED 0.5f
#define GOOMBA_GRAVITY 0.002f
#define GOOMBA_SMALLBOUNCE_DEFLECT_SPEED 0.4f
#define GOOMBA_BIGBOUNCE_DEFLECT_SPEED 0.175f

#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT_DEAD 9

#define GOOMBA_FORM_NORMAL 0
#define GOOMBA_FORM_PARA 1

#define GOOMBA_STATE_WALK_LEFT 100
#define GOOMBA_STATE_WALK_RIGHT 150
#define GOOMBA_STATE_FLATTENED 200
#define GOOMBA_STATE_DEATH 250
#define GOOMBA_STATE_FLY 300

#define GOOMBA_ANI_WALKING 0
#define GOOMBA_ANI_DEATH 1
#define GOOMBA_ANI_PARA_WALKING 2
#define GOOMBA_ANI_PARA_FLY 3

#define GOOMBA_WALK_TIME 1000
#define GOOMBA_FLATTENED_TIME 500

class CGoomba : public CGameObject
{
protected:
	int min_x;
	int max_x;

	int form;

	int bounces;

	int prevState;

	int walk;
	DWORD walk_start;

	DWORD flattened_start;
public:
	CGoomba(int x, int X, bool IsPara);
	virtual void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);
	virtual void SetState(int state);

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();

	int GetEnemyForm() { return this->form; }
	int GetPrevState() { return this->prevState; }
	void SetEnemyForm(int form) { this->form = form; }

	void StartFlattened() { flattened_start = GetTickCount(); }
	void StartWalk() { walk = 1; walk_start = GetTickCount(); }
};