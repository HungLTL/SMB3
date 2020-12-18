#pragma once
#include "BackgroundObject.h"

class CBackgroundTile :public CBackgroundObject {
public:
	void Update(DWORD dt) { return; }
	void Render() { animation_set->at(0)->Render(x, y); }
};