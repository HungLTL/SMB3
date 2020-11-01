#pragma once
#include "GameObject.h"
#include "WoodPlatform.h"

#define PINK 1
#define BLUE 2
#define GREEN 3
#define WHITE 4

#define BP_PINK_TOPLEFT 0
#define BP_PINK_TOPMID 1
#define BP_PINK_TOPRIGHT 2
#define BP_PINK_LEFT 3
#define BP_PINK_MID 4
#define BP_PINK_RIGHT 5
#define BP_PINK_BOTLEFT 6
#define BP_PINK_BOTMID 7
#define BP_PINK_BOTRIGHT 8

#define BP_BLUE_TOPLEFT 9
#define BP_BLUE_TOPMID 10
#define BP_BLUE_TOPRIGHT 11
#define BP_BLUE_LEFT 12
#define BP_BLUE_MID 13
#define BP_BLUE_RIGHT 14
#define BP_BLUE_BOTLEFT 15
#define BP_BLUE_BOTMID 16
#define BP_BLUE_BOTRIGHT 17

#define BP_GREEN_TOPLEFT 18
#define BP_GREEN_TOPMID 19
#define BP_GREEN_TOPRIGHT 20
#define BP_GREEN_LEFT 21
#define BP_GREEN_MID 22
#define BP_GREEN_RIGHT 23
#define BP_GREEN_BOTLEFT 24
#define BP_GREEN_BOTMID 25
#define BP_GREEN_BOTRIGHT 26

#define BP_WHITE_TOPLEFT 27
#define BP_WHITE_TOPMID 28
#define BP_WHITE_TOPRIGHT 29
#define BP_WHITE_LEFT 30
#define BP_WHITE_MID 31
#define BP_WHITE_RIGHT 32
#define BP_WHITE_BOTLEFT 33
#define BP_WHITE_BOTMID 34
#define BP_WHITE_BOTRIGHT 35

class CBackgroundPlatform :public CWoodPlatform {
	int color;
public:
	CBackgroundPlatform(int length, int height, int color) :CWoodPlatform(length, height) {
		this->color = color;
		FillObject();
	}
	void FillObject();
	void GetBoundingBox(float& l, float& t, float& r, float& b);
};