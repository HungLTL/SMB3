#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scene.h"

#include "GameObject.h"
#include "BackgroundPlatform.h"
#include "Block.h"
#include "Mario.h"
#include "Pipe.h"
#include "GoldBlock.h"
#include "PowerBlock.h"
#include "WoodPlatform.h"

#include "BackgroundObject.h"
#include "BackgroundTIle.h"
#include "Cloud.h"
#include "Shrub.h"
#include "BackgroundObjectHeight.h"
#include "BackgroundObjectRect.h"

#include "Fireball.h"

#include "Goomba.h"
#include "Koopa.h"

class CPlayScene :public CScene {
	CMario* player;

	vector<LPGAMEOBJECT> objects;
	vector<LPBGOBJECT> bg_objects;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
public:
	CPlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	CMario* getPlayer() { return player; }
	vector<LPGAMEOBJECT> getObjects() { return objects; }
	void updateObjects(vector<LPGAMEOBJECT> vec) { objects = vec; }
};

class CPlaySceneKeyHandler :public CSceneKeyHandler {
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	CPlaySceneKeyHandler(CScene* scene) :CSceneKeyHandler(scene) {};
};