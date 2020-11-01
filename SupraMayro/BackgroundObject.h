#pragma once
#include <Windows.h>
#include <d3dx9.h>
#include <vector>
#include "Sprites.h"
#include "Animations.h"

#define WIDTH 16

using namespace std;

class CBackgroundObject {
protected:
	float x, y;
	LPANIMATION_SET animation_set;
public:
	CBackgroundObject();

	void SetPosition(float x, float y) { this->x = x; this->y = y; }
	void GetPosition(float& x, float& y) { x = this->x; y = this->y; }

	void SetAnimationSet(LPANIMATION_SET ani_set) { animation_set = ani_set; }

	virtual void Render() = 0;

	~CBackgroundObject();
};
typedef CBackgroundObject* LPBGOBJECT;