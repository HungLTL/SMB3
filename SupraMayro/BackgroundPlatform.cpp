#include "BackgroundPlatform.h"
#include "Game.h"

void CBackgroundPlatform::FillObject() {
	switch (color) {
	case PINK:
		grid[0][0] = BP_PINK_TOPLEFT;
		grid[height - 1][length - 1] = BP_PINK_BOTRIGHT;
		grid[0][length - 1] = BP_PINK_TOPRIGHT;
		grid[height - 1][0] = BP_PINK_BOTLEFT;
		if (length > 2) {
			for (int i = 1; i < length - 1; i++) {
				grid[0][i] = BP_PINK_TOPMID;
				grid[height - 1][i] = BP_PINK_BOTMID;
			}
		}
		if (height > 2) {
			for (int i = 1; i < height - 1; i++) {
				grid[i][0] = BP_PINK_LEFT;
				grid[i][length - 1] = BP_PINK_RIGHT;
			}
		}
		if ((length > 2) && (height > 2)) {
			for (int i = 1; i < height - 1; i++) {
				for (int j = 1; j < length - 1; j++) {
					grid[i][j] = BP_PINK_MID;
				}
			}
		}
		break;
	case BLUE:
		grid[0][0] = BP_BLUE_TOPLEFT;
		grid[height - 1][length - 1] = BP_BLUE_BOTRIGHT;
		grid[0][length - 1] = BP_BLUE_TOPRIGHT;
		grid[height - 1][0] = BP_BLUE_BOTLEFT;
		if (length > 2) {
			for (int i = 1; i < length - 1; i++) {
				grid[0][i] = BP_BLUE_TOPMID;
				grid[height - 1][i] = BP_BLUE_BOTMID;
			}
		}
		if (height > 2) {
			for (int i = 1; i < height - 1; i++) {
				grid[i][0] = BP_BLUE_LEFT;
				grid[i][length - 1] = BP_BLUE_RIGHT;
			}
		}
		if ((length > 2) && (height > 2)) {
			for (int i = 1; i < height - 1; i++) {
				for (int j = 1; j < length - 1; j++) {
					grid[i][j] = BP_BLUE_MID;
				}
			}
		}
		break;
	case GREEN:
		grid[0][0] = BP_GREEN_TOPLEFT;
		grid[height - 1][length - 1] = BP_GREEN_BOTRIGHT;
		grid[0][length - 1] = BP_GREEN_TOPRIGHT;
		grid[height - 1][0] = BP_GREEN_BOTLEFT;
		if (length > 2) {
			for (int i = 1; i < length - 1; i++) {
				grid[0][i] = BP_GREEN_TOPMID;
				grid[height - 1][i] = BP_GREEN_BOTMID;
			}
		}
		if (height > 2) {
			for (int i = 1; i < height - 1; i++) {
				grid[i][0] = BP_GREEN_LEFT;
				grid[i][length - 1] = BP_GREEN_RIGHT;
			}
		}
		if ((length > 2) && (height > 2)) {
			for (int i = 1; i < height - 1; i++) {
				for (int j = 1; j < length - 1; j++) {
					grid[i][j] = BP_GREEN_MID;
				}
			}
		}
		break;
	case WHITE:
		grid[0][0] = BP_WHITE_TOPLEFT;
		grid[height - 1][length - 1] = BP_WHITE_BOTRIGHT;
		grid[0][length - 1] = BP_WHITE_TOPRIGHT;
		grid[height - 1][0] = BP_WHITE_BOTLEFT;
		if (length > 2) {
			for (int i = 1; i < length - 1; i++) {
				grid[0][i] = BP_WHITE_TOPMID;
				grid[height - 1][i] = BP_WHITE_BOTMID;
			}
		}
		if (height > 2) {
			for (int i = 1; i < height - 1; i++) {
				grid[i][0] = BP_WHITE_LEFT;
				grid[i][length - 1] = BP_WHITE_RIGHT;
			}
		}
		if ((length > 2) && (height > 2)) {
			for (int i = 1; i < height - 1; i++) {
				for (int j = 1; j < length - 1; j++) {
					grid[i][j] = BP_WHITE_MID;
				}
			}
		}
		break;
	}
}

void CBackgroundPlatform::GetBoundingBox(float& left, float& top, float& right, float& bottom) {
	left = x;
	top = y;
	right = x + (length * WIDTH);
	bottom = y + (height * WIDTH);
}