#pragma once
#include "GameObject.h"
#include "Goomba.h"

#define KOOPA_WALKING_SPEED 0.03f
#define KOOPA_PINBALL_SPEED 0.2f
#define PARATROOPA_BOUNCE_SPEED 0.45f
#define KOOPA_DIE_DEFLECT_SPEED 0.5f
#define KOOPA_GRAVITY 0.0015f

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
#define PARATROOPA_ANI_LEFT 5
#define PARATROOPA_ANI_RIGHT 6

#define DORMANT_TIME 10000

class CKoopa :public CGameObject {
protected:
	int min_x;
	int max_x;

	int playerX, playerY;

	bool Para;
	bool IsBeingCarried;

	int dormant;
	DWORD dormant_start;
public:
	CKoopa(int x, int X);
	CKoopa(int x, int X, int px, int py);
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