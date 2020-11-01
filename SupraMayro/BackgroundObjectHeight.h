#pragma once
#include "BackgroundObject.h"

class CBackgroundObjectHeight :public CBackgroundObject {
protected:
	int height;
public:
	CBackgroundObjectHeight(int height) :CBackgroundObject() {
		this->height = height;
	}
	virtual void Render();
};