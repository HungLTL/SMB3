#pragma once
#include "GameObject.h"
#include "Pipe.h"

#define PLATFORM_UPPER_LCORNER 0
#define PLATFORM_UPPER_MID 1
#define PLATFORM_UPPER_RCORNER 2
#define PLATFORM_SIDE_LEFT 3
#define PLATFORM_MID 4
#define PLATFORM_SIDE_RIGHT 5

class CWoodPlatform :public CPipe {
protected:
	int length;
public:
	CWoodPlatform(int length, int height) {
		this->height = height;
		this->length = length;
		vector<vector<int> > vec(height, vector<int>(length));
		this->grid = vec;
		FillObject();
	}
	virtual void FillObject();
	virtual void Render();
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);

	int GetLength() { return this->length * WIDTH; }
};