#include "Grid.h"

#include "Mario.h"

#include "Game.h"
#include "PlayScene.h"

CCell::CCell(float x, float y, int i, int j) {
	coords.row = i;
	coords.column = j;

	left = x;
	top = y;
	right = x + CELL_WIDTH;
	bottom = y + CELL_WIDTH;
}

bool CCell::BelongsToCell(LPGAMEOBJECT obj) {
	float fx, fy;
	obj->GetPosition(fx, fy);

	if ((fx >= left) && (fx < right) && (fy >= top) && (fy < bottom))
		return true;
	else
		return false;
}

CGrid::CGrid(float l, float h, float StartX, float StartY) {
	length = l / CELL_WIDTH;
	height = h / CELL_WIDTH;
	vector<vector<LPCELL>> vec(height, vector<LPCELL>(length));
	grid = vec;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < length; j++) {
			grid[i][j] = new CCell(StartX + CELL_WIDTH * j, StartY + CELL_WIDTH * i, i, j);
		}
	}

	CTextures::GetInstance()->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));
}

void CGrid::GetAdjacentCells(LPCELL cell, vector<LPCELL> &result) {
	CCoordinates coords = cell->GetCoords();

	if (coords.column > 0)
		result.push_back(grid[coords.row][coords.column - 1]);

	if (coords.column < length - 1)
		result.push_back(grid[coords.row][coords.column + 1]);

	if (coords.row > 0)
		result.push_back(grid[coords.row - 1][coords.column]);

	if (coords.row < height - 1)
		result.push_back(grid[coords.row + 1][coords.column]);
}

LPCELL CGrid::LocateCellByObject(LPGAMEOBJECT obj) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < length; j++) {
			if (grid[i][j]->BelongsToCell(obj))
				return grid[i][j];
		}
	}
	return NULL;
}