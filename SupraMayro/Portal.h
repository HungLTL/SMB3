#pragma once
#include "GameObject.h"

class CPortal : public CGameObject {
	int scene_id;
	int width, height;
	int newX, newY;
	bool Direction;
public:
	CPortal(int w, int h, int scene_id, int newX, int newY, int dir);

	int GetSceneID() { return scene_id; }
	int GetNewX() { return newX; }
	int GetNewY() { return newY; }
	bool GetDirection() { return Direction; }

	void GetBoundingBox(float& l, float& t, float& r, float& b);

	void Render() { return; }
};