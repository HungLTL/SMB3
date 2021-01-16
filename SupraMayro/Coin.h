#pragma once
#include "GameObject.h"

#define COIN_STATE_STATIC 0
#define COIN_STATE_ACTIVE 200

#define COIN_ANI_STATIC 0
#define COIN_ANI_ACTIVE 1

#define COIN_BBOX_WIDTH 14
#define COIN_BBOX_HEIGHT 14

#define COIN_GRAVITY 0.075f
#define COIN_BOUNCE 0.7f

class CCoin :public CGameObject {
protected:
	bool TransformedFromBlock;
public:
	CCoin(int type);

	void SetOrigins(bool value) { this->TransformedFromBlock = value; }
	bool GetOrigins() { return this->TransformedFromBlock; }

	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);

	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	void Render();
	void SetState(int state);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
};