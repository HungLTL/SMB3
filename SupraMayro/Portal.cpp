#include "Portal.h"

#include "Game.h"

CPortal::CPortal(int w, int h, int scene_id, int newX, int newY, int dir)
{
	this->width = w;
	this->height = h;
	this->scene_id = scene_id;
	this->newX = newX;
	this->newY = newY;
	if (dir == 0)
		Direction = false;
	else
		Direction = true;
}

void CPortal::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = x;
	t = y;
	r = x + width;
	b = y + height;
}