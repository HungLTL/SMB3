#pragma once
#include "BackgroundObject.h"

#define CLOUD_TOPLEFT 0
#define CLOUD_TOPMID 1
#define CLOUD_TOPRIGHT 2
#define CLOUD_BOTLEFT 3
#define CLOUD_BOTMID 4
#define CLOUD_BOTRIGHT 5

class CCloud :public CBackgroundObject {
protected:
	int length;
	vector<vector<int>> grid;
public:
	CCloud(int length) :CBackgroundObject() {
		this->length = length;
		vector<vector<int> > vec(2, vector<int>(length));
		this->grid = vec;
		FillObject();
	}
	virtual void FillObject();
	void Update(DWORD dt) { return; }
	virtual void Render();
};