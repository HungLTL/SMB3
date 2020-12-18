#pragma once
#include "GameObject.h"

#define WIDTH 16

#define PIPE_OVERWORLD 0
#define PIPE_UNDERWORLD 1

#define PIPE_TOPLEFT 0
#define PIPE_TOPRIGHT 1
#define PIPE_LEFT 2
#define PIPE_RIGHT 3
#define PIPE_UNDERWORLD_TOPLEFT 4
#define PIPE_UNDERWORLD_TOPRIGHT 5
#define PIPE_UNDERWORLD_LEFT 6
#define PIPE_UNDERWORLD_RIGHT 7

class CPipe :public CGameObject {
private:
	int type;
	bool IsWarpPipe;
protected:
	int height;
	vector<vector<int>> grid;
public:
	CPipe() { height = type = NULL; IsWarpPipe = false; }
	CPipe(int height, int type, int warp) :CGameObject() {
		this->height = height;
		this->type = type;
		if (warp == 0)
			IsWarpPipe = false;
		else
			IsWarpPipe = true;

		vector<vector<int> > vec(height, vector<int>(2));
		this->grid = vec;
		FillObject();
	}

	bool GetWarp() { if (IsWarpPipe) return true; else return false; }

	virtual void FillObject();
	virtual void Render();
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};