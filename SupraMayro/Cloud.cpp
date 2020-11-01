#include "Cloud.h"
#include "Game.h"

void CCloud::FillObject() {
	grid[0][0] = CLOUD_TOPLEFT;
	grid[0][length - 1] = CLOUD_TOPRIGHT;
	grid[1][0] = CLOUD_BOTLEFT;
	grid[1][length - 1] = CLOUD_BOTRIGHT;
	for (int i = 1; i < length - 1; i++) {
		grid[0][i] = CLOUD_TOPMID;
		grid[1][i] = CLOUD_BOTMID;
	}
}

void CCloud::Render() {
	for (int i = 0; i < length; i++) {
		animation_set->at(grid[0][i])->Render(x + (i * (WIDTH)), y);
		animation_set->at(grid[1][i])->Render(x + (i * (WIDTH)), y + WIDTH);
	}
}