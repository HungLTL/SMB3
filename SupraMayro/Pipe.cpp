#include "Pipe.h"
#include "Game.h"

void CPipe::FillObject() {
	grid[0][0] = PIPE_TOPLEFT;
	grid[0][1] = PIPE_TOPRIGHT;
	for (int i = 1; i < height; i++) {
		grid[i][0] = PIPE_LEFT;
		grid[i][1] = PIPE_RIGHT;
	}
}

void CPipe::Render() {
	for (int i = 0; i < height; i++) {
		animation_set->at(grid[i][0])->Render(x, y + (i * WIDTH));
		animation_set->at(grid[i][1])->Render(x + WIDTH, y + (i * WIDTH));
	}
}

void CPipe::GetBoundingBox(float& left, float& top, float& right, float& bottom) {
	left = x;
	top = y;
	right = x + WIDTH * 2;
	bottom = y + (WIDTH * height);
}