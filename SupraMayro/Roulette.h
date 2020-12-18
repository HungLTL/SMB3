#pragma once
#include "GameObject.h"

#define ROULETTE_CARD_MUSHROOM 0
#define ROULETTE_CARD_FLOWER 1
#define ROULETTE_CARD_STAR 2

#define ROULETTE_STATE_IDLE 0
#define ROULETTE_STATE_ACTIVATED 1
#define ROULETTE_STATE_DISPLAY 2

#define ROULETTE_ANI_MUSHROOM 0
#define ROULETTE_ANI_MUSHROOM_ACTIVATED 1
#define ROULETTE_ANI_MUSHROOM_CARD 2
#define ROULETTE_ANI_FLOWER 3
#define ROULETTE_ANI_FLOWER_ACTIVATED 4
#define ROULETTE_ANI_FLOWER_CARD 5
#define ROULETTE_ANI_STAR 6
#define ROULETTE_ANI_STAR_ACTIVATED 7
#define ROULETTE_ANI_STAR_CARD 8
#define ROULETTE_ANI_VICTORY_SCREEN 9

#define CYCLE_TIME 333

#define ROULETTE_BBOX_WIDTH 26
#define ROULETTE_ACTIVATE_SPEED 0.65f

class CRoulette :public CGameObject {
	int type;

	DWORD cycle_start;
public:
	CRoulette();

	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);

	void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	void Render();
	void GetBoundingBox(float& l, float& t, float& r, float& b);

	void SetState(int state);

	void StartCycle() { cycle_start = GetTickCount(); }
};