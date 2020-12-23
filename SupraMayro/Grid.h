#pragma once
#include "GameObject.h"

#define CELL_WIDTH 256

struct CCoordinates {
	int row, column;
};

class CCell {
	CCoordinates coords;
	float left, top, right, bottom;
public:
	CCell(float x, float y, int i, int j);

	CCoordinates GetCoords() { return this->coords; }

	bool BelongsToCell(LPGAMEOBJECT obj);
};

typedef CCell* LPCELL;

class CGrid {
	int length;
	int height;
	vector<vector<LPCELL>> grid;
public:
	CGrid(float l, float h, float StartX, float StartY);

	void GetAdjacentCells(LPCELL cell, vector<LPCELL>& result);
	LPCELL LocateCellByObject(LPGAMEOBJECT obj);
};