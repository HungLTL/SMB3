#include "WoodPlatform.h"
#include "Game.h"

void CWoodPlatform::FillObject() {
	grid[0][0] = PLATFORM_UPPER_LCORNER;
	grid[0][length - 1] = PLATFORM_UPPER_RCORNER;
	if (height > 1) {
		for (int i = 1; i < height; i++) {
			grid[i][0] = PLATFORM_SIDE_LEFT;
			grid[i][length - 1] = PLATFORM_SIDE_RIGHT;
		}
	}
	if (length > 2) {
		for (int i = 1; i < length - 1; i++) {
			grid[0][i] = PLATFORM_UPPER_MID;
			if (height > 1) {
				for (int j = 1; j < height; j++)
					grid[j][i] = PLATFORM_MID;
			}
		}
	}
}

void CWoodPlatform::Render() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < length; j++) {
			animation_set->at(grid[i][j])->Render(x + (j * WIDTH), y + (i * WIDTH));
		}
	}
}

void CWoodPlatform::GetBoundingBox(float& left, float& top, float& right, float& bottom) {
	left = x;
	top = y;
	right = x + (length * WIDTH);
	bottom = y + (height * WIDTH);
}