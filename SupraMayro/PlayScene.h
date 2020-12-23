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

#include "BackgroundObject.h"
#include "BackgroundTile.h"
#include "Cloud.h"
#include "Shrub.h"
#include "BackgroundObjectHeight.h"
#include "BackgroundObjectRect.h"

#include "Fireball.h"

#include "Coin.h"
#include "PowerUp.h"

#include "Goomba.h"
#include "Koopa.h"
#include "PiranhaPlant.h"

#include "Roulette.h"
#include "Portal.h"

#include "HUD.h"

#define TIC 1000
#define COURSE_END_DELAY 4000

class CPlayScene :public CScene {
	CMario* player;
	CHUD* hud;

	CGrid* grid;

	vector<LPGAMEOBJECT> objects;
	vector<LPBGOBJECT> bg_objects;

	float minX, maxX, minY, maxY;
	bool CourseCompleted;

	int timer;
	DWORD timer_start;
	DWORD course_end;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
public:
	CPlayScene(int id, LPCWSTR filePath, float x, float X, float y, float Y);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	CMario* getPlayer() { return player; }
	vector<LPGAMEOBJECT> getObjects() { return objects; }

	void PushObject(LPGAMEOBJECT obj) { objects.push_back(obj); }
	void Replace(int i, LPGAMEOBJECT obj) { objects[i] = obj; }
	void RemoveObject(LPGAMEOBJECT obj) { objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end()); }

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