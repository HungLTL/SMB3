#pragma once
#include "BackgroundObject.h"

#define SHRUB_TOPLEFT 0
#define SHRUB_TOPRIGHT 1
#define SHRUB_LEFT 2
#define SHRUB_RIGHT 3
#define SHRUB_MID 4
#define SHRUB_LEFT_INLAY 5
#define SHRUB_RIGHT_INLAY 6
#define SHRUB_TOPLEFT_FINISH 7
#define SHRUB_TOPRIGHT_FINISH 8
#define SHRUB_LEFT_FINISH 9
#define SHRUB_RIGHT_FINISH 10
#define SHRUB_MID_FINISH 11

class CShrub :public CBackgroundObject {
protected:
	vector<int> data;
public:
	CShrub(vector<int> info);
	void Update(DWORD dt) { return; }
	void Render();
};