#pragma once
#include "GameObject.h"
#include "Block.h"

#define PSWITCH_STATE_HIDDEN 0
#define PSWITCH_STATE_IDLE 100
#define PSWITCH_STATE_ACTIVATED 200

#define PSWITCH_ANI_OFF 0
#define PSWITCH_ANI_ON 1
#define PSWITCH_ANI_HIDDEN 2 // shiny gold block
#define PSWITCH_ANI_BASE 3 // deactivated power block

#define TRANSMUTE_TIME 10000

#define PSWITCH_BBOX_HEIGHT 32

class CPSwitch :public CBlock {
protected:
	int transmute;
	DWORD transmute_start;
public:
	CPSwitch();

	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	void Render();

	void StartTransmute() { transmute = 1; transmute_start = GetTickCount(); }

	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
};