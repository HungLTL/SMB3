#pragma once
#include "GameObject.h"

#define TAIL_STATE_DORMANT 0
#define TAIL_STATE_ATTACK_LEFT 100
#define TAIL_STATE_ATTACK_RIGHT 200

#define TAIL_BBOX_WIDTH 9
#define TAIL_BBOX_HEIGHT 6

#define TAIL_WINDUP_TIME 60
#define TAIL_FIRSTNEUTRAL_TIME 120
#define TAIL_ATTACK_TIME 180
#define TAIL_SECONDNEUTRAL_TIME 240
#define TAIL_SECONDATTACK_TIME 300

class CTail :public CGameObject {
protected:
	int attack;
	DWORD attack_start;

	int prevState;
public:
	CTail();

	bool CheckOverlap(LPGAMEOBJECT obj);

	void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	void Render() {}
	void StartAttack() { attack = 1; attack_start = GetTickCount(); }

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void SetState(int state);
};