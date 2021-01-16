#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scene.h"
#include "Grid.h"

#include "GameObject.h"
#include "BackgroundPlatform.h"
#include "Block.h"
#include "Mario.h"
#include "Tail.h"
#include "Pipe.h"
#include "GoldBlock.h"
#include "PSwitch.h"
#include "PowerBlock.h"
#include "WoodPlatform.h"
#include "CoinBlock.h"
#include "Lift.h"

#include "BackgroundObject.h"
#include "BackgroundTile.h"
#include "Cloud.h"
#include "Shrub.h"
#include "BackgroundObjectHeight.h"
#include "BackgroundObjectRect.h"

#include "Fireball.h"
#include "Boomerang.h"

#include "Coin.h"
#include "PowerUp.h"

#include "Goomba.h"
#include "Koopa.h"
#include "PiranhaPlant.h"
#include "BoomerangBro.h"

#include "Roulette.h"
#include "Portal.h"

#include "HUD.h"

#define TIC 1000
#define COURSE_END_DELAY 4000

#define CAMERA_SCROLL_SPEED 0.45f

class CPlayScene :public CScene {
	CMario* player;
	CTail* tail;
	CHUD* hud;

	CGrid* grid;

	int xOnMap, yOnMap;
	float minX, maxX, minY, maxY;
	bool CourseCompleted;
	bool AutoScroll;

	int NumOfFireballs;

	int timer;
	DWORD timer_start;
	DWORD course_end;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_CELLS(string line, int row, int column);
public:
	CPlayScene(int id, LPCWSTR filePath, float x, float X, float y, float Y, int xOnMap, int yOnMap, int scroll);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	CMario* getPlayer() { return player; }
	void getCells(LPGAMEOBJECT obj, vector<LPCELL> &adjCells) { LPCELL cell = grid->LocateCellByObject(obj); grid->GetAdjacentCells(cell, adjCells); }

	void PushObject(LPGAMEOBJECT obj);
	void Replace(int i, LPGAMEOBJECT newObj);
	void RemoveObject(LPGAMEOBJECT obj);

	void AddFireball() { NumOfFireballs++; }
	void RemoveFireball() { NumOfFireballs--; }
	int GetFireballs() { return NumOfFireballs; }

	void StartTimer() { timer = 1; timer_start = GetTickCount(); }
	void EndCourse();
	bool GetCourseStatus() { return CourseCompleted; }
};

class CPlaySceneKeyHandler :public CSceneKeyHandler {
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	CPlaySceneKeyHandler(CScene* scene) :CSceneKeyHandler(scene) {};
};