#pragma once
#include "BackgroundObject.h"
#include "BackgroundObjectHeight.h"

class CBackgroundObjectRect :public CBackgroundObjectHeight {
protected:
	int length;
public:
	CBackgroundObjectRect(int length, int height) :CBackgroundObjectHeight(height) { this->length = length; }
	void Render();
};