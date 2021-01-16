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

void CCell::ClearCell() {
	for (size_t i = 0; i < objects.size(); i++)
		delete objects[i];

	for (size_t j = 0; j < bg_objects.size(); j++)
		delete bg_objects[j];

	objects.clear();
	bg_objects.clear();
}

CGrid::CGrid(float l, float h, float StartX, float StartY) {
	length = (int)(l / CELL_WIDTH);
	height = (int)(h / CELL_WIDTH);
	vector<vector<LPCELL>> vec(height, vector<LPCELL>(length));
	grid = vec;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < length; j++) {
			grid[i][j] = new CCell(StartX + CELL_WIDTH * j, StartY + CELL_WIDTH * i, i, j);
		}
	}
}

void CGrid::ReplaceObject(int row, int column, LPGAMEOBJECT obj, LPGAMEOBJECT newObj) {
	for (size_t i = 0; i < grid[row][column]->GetObjects().size(); i++) {
		if (grid[row][column]->GetObjects()[i] == obj) {
			grid[row][column]->GetObjects()[i] = newObj;
			return;
		}
	}
}

bool CGrid::CheckObjectIntegrity(LPGAMEOBJECT obj, LPCELL cell) {
	if (cell->BelongsToCell(obj))
		return true;
	else {
		vector<LPCELL> PotentialNewCells;
		GetAdjacentCells(cell, PotentialNewCells);

		for (size_t i = 0; i < PotentialNewCells.size(); i++) {
			if (PotentialNewCells[i]->BelongsToCell(obj)) {
				cell->DeleteObject(obj);
				PotentialNewCells[i]->AddToCell(obj);
				return true;
			}
		}

		return false;
	}
}

void CGrid::GetAdjacentCells(LPCELL cell, vector<LPCELL> &result) {
	CCoordinates coords = cell->GetCoords();

	if (coords.row > 0) {
		if (coords.column > 0)
			result.push_back(grid[coords.row - 1][coords.column - 1]);

		result.push_back(grid[coords.row - 1][coords.column]);

		if (coords.column < length - 1)
			result.push_back(grid[coords.row - 1][coords.column + 1]);
	}

	if (coords.column > 0)
		result.push_back(grid[coords.row][coords.column - 1]);

	result.push_back(grid[coords.row][coords.column]);

	if (coords.column < length - 1)
		result.push_back(grid[coords.row][coords.column + 1]);

	if (coords.row < height - 1) {
		if (coords.column > 0)
			result.push_back(grid[coords.row + 1][coords.column - 1]);

		result.push_back(grid[coords.row + 1][coords.column]);

		if (coords.column < length - 1)
			result.push_back(grid[coords.row + 1][coords.column + 1]);
	}
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

void CGrid::ClearGrid() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < length; j++)
			grid[i][j]->ClearCell();
	}
}