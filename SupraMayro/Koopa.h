#pragma once
#include "GameObject.h"
#include "Goomba.h"

#define KOOPA_WALKING_SPEED 0.03f
#define PARAKOOPA_WALKING_SPEED 0.03f
#define KOOPA_PINBALL_SPEED 0.2f
#define PARATROOPA_BOUNCE_SPEED 0.275f
#define KOOPA_DIE_DEFLECT_SPEED 0.5f
#define KOOPA_GRAVITY 0.0015f
#define PARAKOOPA_GRAVITY 0.001f
#define KOOPA_SLOWFALL 0.00138f
#define KOOPA_FLY_SPEED 0.14f
#define KOOPA_BOUNCE_RECOIL 0.055f

#define KOOPA_BBOX_WIDTH 16
#define KOOPA_BBOX_HEIGHT 26
#define KOOPA_SHELL_BBOX_HEIGHT 15

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
#define PARATROOPA_ANI_LEFT 5
#define PARATROOPA_ANI_RIGHT 6
#define KOOPA_ANI_EMERGING 7

#define DORMANT_TIME 10000
#define DORMANT_WARNING 8000

class CKoopa :public CGameObject {
protected:
	int min_x, max_x;

	int playerx, playerX;
	int min_y, max_y;

	bool Para, IsBeingCarried, active, IsStationary, IsFlyingUp;

	int dormant;
	DWORD dormant_start;
public:
	CKoopa(int x, int X);
	CKoopa(int x, int X, int px, int pX);
	CKoopa(int x, int y, int Y);
	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);
	void SetState(int state);
	int GetState() { return this->state; }
	void StartDormant() { dormant = 1; dormant_start = GetTickCount(); }
	bool GetPara() { if (Para) return true; else return false; }
	void SetPara(bool val) { this->Para = val; }

	void SetCarryStatus(bool value) { this->IsBeingCarried = value; }

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	void Render();
};