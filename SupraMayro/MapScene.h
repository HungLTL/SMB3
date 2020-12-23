#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scene.h"

#include "GameObject.h"
#include "MapMario.h"
#include "Block.h"
#include "MapLevelBlock.h"
#include "Mario.h"

#include "BackgroundObject.h"
#include "BackgroundTile.h"
#include "BackgroundObjectRect.h"
#include "GameOver.h"

#include "HUD.h"

class CMapScene :public CScene {
	CMapMario* mario;
	CHUD* hud;
	CGameOver* gameover;

	int destination_scene;
	bool IsGameOver;

	vector<LPGAMEOBJECT> objects;
	vector<LPBGOBJECT> bg_objects;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
public:
	CMapScene(int id, LPCWSTR filePath);

	void Load();
	void Update(DWORD dt);
	void Render();
	void Unload();

	bool GetGameStatus() { return IsGameOver; }
	CMapMario* GetPlayer() { return this->mario; }
	CGameOver* GetGameOverInterface() { return this->gameover; }
	void SetDestinationScene(int value) { this->destination_scene = value; }
	int GetDestinationScene() { return this->destination_scene; }
};

class CMapSceneKeyHandler :public CSceneKeyHandler {
public:
	void KeyState(BYTE* states) { return; }
	void OnKeyDown(int KeyCode);
	void OnKeyUp(int KeyCode) { return; }
	CMapSceneKeyHandler(CScene* scene) :CSceneKeyHandler(scene) {};
};