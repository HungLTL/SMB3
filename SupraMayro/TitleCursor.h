#pragma once
#include "BackgroundTile.h"

#define CURSOR_SP 0
#define CURSOR_MP 1

class CTitleCursor :public CBackgroundTile {
private:
	int state;
public:
	CTitleCursor() :CBackgroundTile() { this->state = CURSOR_SP; }
	void ChangeState();
	int GetState() { return state; }
};