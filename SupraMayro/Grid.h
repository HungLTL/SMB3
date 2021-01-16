#pragma once
#include "GameObject.h"
#include "BackgroundObject.h"

#define CELL_WIDTH 256

struct CCoordinates {
	int row, column;
};

class CCell {
	CCoordinates coords;
	float left, top, right, bottom;

	vector<LPGAMEOBJECT> objects;
	vector<LPBGOBJECT> bg_objects;
public:
	CCell(float x, float y, int i, int j);

	CCoordinates GetCoords() { return this->coords; }

	void AddToCell(LPGAMEOBJECT obj) {this->objects.push_back(obj); }
	void MoveToCell(LPGAMEOBJECT obj) { this->objects.push_back(std::move(obj)); }
	void AddToCell(LPBGOBJECT bg_obj) { this->bg_objects.push_back(bg_obj); }

	void DeleteObject(LPGAMEOBJECT obj) { objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end()); }

	void Replace(int index, LPGAMEOBJECT new_obj) { objects[index] = new_obj; }

	bool BelongsToCell(LPGAMEOBJECT obj);

	vector<LPGAMEOBJECT> GetObjects() { return objects; }
	vector<LPBGOBJECT> GetBackgroundObjects() { return bg_objects; }

	void ClearCell();
};

typedef CCell* LPCELL;

class CGrid {
	int length;
	int height;
	vector<vector<LPCELL>> grid;
public:
	CGrid(float l, float h, float StartX, float StartY);

	void AddToGrid(int row, int column, LPGAMEOBJECT obj) { grid[row][column]->AddToCell(obj); }
	void AddToGrid(int row, int column, LPBGOBJECT bg_obj) { grid[row][column]->AddToCell(bg_obj); }

	void ReplaceObject(int row, int column, LPGAMEOBJECT obj, LPGAMEOBJECT newObj);

	bool CheckObjectIntegrity(LPGAMEOBJECT obj, LPCELL cell);

	void GetAdjacentCells(LPCELL cell, vector<LPCELL>& result);
	LPCELL LocateCellByObject(LPGAMEOBJECT obj);

	void ClearGrid();
};