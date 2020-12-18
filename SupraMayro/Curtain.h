#pragma once
#include "BackgroundObjectHeight.h"

#define CURTAIN_BODY 0
#define CURTAIN_EDGE 1

#define CURTAIN_STATE_IDLE 0
#define CURTAIN_STATE_FREEZE 50
#define CURTAIN_STATE_UNVEIL 100

#define CURTAIN_OPENING_TIME 1000
#define CURTAIN_UNVEIL_SPEED 0.09f

class CCurtain :public CBackgroundObjectHeight {
private:
	float vy;

	DWORD unveil_start;
	vector<int> data;
public:
	CCurtain(int height);

	void Update(DWORD dt);
	void Render();

	void StartUnveil() { unveil_start = GetTickCount(); }

};