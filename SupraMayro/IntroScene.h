#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scene.h"

#include "BackgroundObject.h"
#include "Curtain.h"
#include "TitleCursor.h"

class CIntroScene :public CScene {
	CTitleCursor* cursor;

	vector<LPBGOBJECT> objects;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
public:
	CIntroScene(int id, LPCWSTR filePath);

	void Load();
	void Update(DWORD dt);
	void Render();
	void Unload();

	CTitleCursor* getCursor() { return cursor; }
	void SetCursor(CTitleCursor* cursor);
};

class CIntroSceneKeyHandler :public CSceneKeyHandler {
public:
	void OnKeyDown(int keyCode);
	void OnKeyUp(int keyCode) { return; }
	void KeyState(BYTE* states) { return; }
	CIntroSceneKeyHandler(CScene* scene) :CSceneKeyHandler(scene) {};
};