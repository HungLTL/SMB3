#include "Portal.h"

#include "Game.h"

CPortal::CPortal(int w, int h, int scene_id, int newX, int newY)
{
	this->width = w;
	this->height = h;
	this->scene_id = scene_id;
	this->newX = newX;
	this->newY = newY;
}

void CPortal::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = x;
	t = y;
	r = x + width;
	b = y + height;
}