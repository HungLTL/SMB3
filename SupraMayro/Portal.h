#pragma once
#include "GameObject.h"

class CPortal : public CGameObject {
	int scene_id;
	int width, height;
	int newX, newY;
public:
	CPortal(int w, int h, int scene_id, int newX, int newY);

	int GetSceneID() { return scene_id; }
	int GetNewX() { return newX; }
	int GetNewY() { return newY; }

	void GetBoundingBox(float& l, float& t, float& r, float& b);

	void Render() { return; }
};