#pragma once
#include "GameObject.h"

#define TAIL_BBOX_WIDTH 9
#define TAIL_BBOX_HEIGHT 6

#define TAIL_DAMAGE_TIME 120
#define TAIL_DOWN_TIME 180
#define TAIL_ATTACK_SEQ 300

class CTail :public CGameObject {
protected:
	bool active;
	int attack;
	DWORD attack_start;
public:
	CTail() { attack = 0; attack_start = 0; active = false; }

	bool CheckOverlap(LPGAMEOBJECT obj);

	void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	void Render() { if (active) RenderBoundingBox(); }
	void StartAttack() { attack = 1; attack_start = GetTickCount(); }

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};