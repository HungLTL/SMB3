#pragma once
#include "GameObject.h"

#define STATE_MOVE_UP 100
#define STATE_MOVE_LEFT 200
#define STATE_MOVE_DOWN 300
#define STATE_MOVE_RIGHT 400

#define FORM_NORMAL 1
#define FORM_SUPER 2
#define FORM_RACCOON 3
#define FORM_FIRE 4

#define ANI_NORMAL 0
#define ANI_SUPER 1
#define ANI_RACCOON 2
#define ANI_FIRE 3

#define MOVESPEED_HORIZONTAL 0.20451f
#define MOVESPEED_VERTICAL 0.19f

#define BBOX_WIDTH 14
#define BBOX_HEIGHT 12

#define MOVE_TIME 140

class CMapMario :public CGameObject {
	int form;
	DWORD move_start;
public:
	CMapMario();

	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	void Render();

	void SetPCForm(int value) { this->form = value; }
	int GetPCForm() { return this->form; }

	void StartMove(){ move_start = GetTickCount(); }

	bool IsIdle() { if ((vx == 0) && (vy == 0)) return true; else return false; }

	void SetState(int state);

	void GetBoundingBox(float& l, float& t, float& r, float& b);
};