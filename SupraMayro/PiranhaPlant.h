#pragma once
#include "GameObject.h"
#include "Mario.h"
#include "Fireball.h"

#define PIRANHA_STATE_DORMANT 0
#define PIRANHA_STATE_EMERGE 100
#define PIRANHA_STATE_ATTACK 200
#define PIRANHA_STATE_HIDE 300

#define PIRANHA_TYPE_NORMAL 0
#define PIRANHA_TYPE_FIRE 1

#define PIRANHA_ANI_LEFT 0
#define PIRANHA_ANI_RIGHT 1
#define PIRANHA_ANI_UP 2
#define PIRANHA_ANI_BITE 3

#define PIRANHA_EMERGE_SPEED 0.03f

#define PIRANHA_DORMANT_TIME 4000
#define PIRANHA_ATTACK_TIME 2000

#define PIRANHA_BBOX_HEIGHT 32
#define PIRANHA_BBOX_WIDTH 16

class CPiranhaPlant :public CGameObject {
	int type;

	int attack;
	DWORD attack_start;

	bool IsAttacking;

	int dormant;
	DWORD dormant_start;
public:
	CPiranhaPlant(int type);

	void SetState(int state);

	void StartAttack() { attack = 1; attack_start = GetTickCount(); }
	void StartDormant() { dormant = 1; dormant_start = GetTickCount(); }

	int GetNX() { return this->nx; }

	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	void Render();
};