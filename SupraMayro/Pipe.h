#pragma once
#include "GameObject.h"

#define WIDTH 16

#define PIPE_TOPLEFT 0
#define PIPE_TOPRIGHT 1
#define PIPE_LEFT 2
#define PIPE_RIGHT 3

class CPipe :public CGameObject {
protected:
	int height;
	vector<vector<int>> grid;
public:
	CPipe(int height) :CGameObject() {
		this->height = height;
		vector<vector<int> > vec(height, vector<int>(2));
		this->grid = vec;
		FillObject();
	}
	virtual void FillObject();
	virtual void Render();
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};