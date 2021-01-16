#include <iostream>
#include <fstream>

#include "IntroScene.h"
#include "Utility.h"
#include "Textures.h"
#include "Sprites.h"
#include "Animations.h"

CIntroScene::CIntroScene(int id, LPCWSTR filePath) :CScene(id, filePath) {
	key_handler = new CIntroSceneKeyHandler(this);
	cursor = NULL;
}

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_TEXTURES 2
#define SCENE_SECTION_SPRITES 3
#define SCENE_SECTION_ANIMATIONS 4
#define SCENE_SECTION_ANIMATION_SETS 5
#define SCENE_SECTION_OBJECTS 6

#define OBJECT_TYPE_MENU_BG 0
#define OBJECT_TYPE_CURTAIN 1
#define OBJECT_TYPE_CURSOR 2

#define MAX_SCENE_LINE 1024

using namespace std;

void CIntroScene::_ParseSection_TEXTURES(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 5) return;

	int texID = atoi(tokens[0].c_str());
	wstring path = ToWSTR(tokens[1]);
	int R = atoi(tokens[2].c_str());
	int G = atoi(tokens[3].c_str());
	int B = atoi(tokens[4].c_str());

	CTextures::GetInstance()->Add(texID, path.c_str(), D3DCOLOR_XRGB(R, G, B));
}

void CIntroScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return;

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPDIRECT3DTEXTURE9 tex = CTextures::GetInstance()->Get(texID);

	CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void CIntroScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return;

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (size_t i = 1; i < tokens.size(); i += 2)
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i + 1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
}

void CIntroScene::_ParseSection_ANIMATION_SETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return;

	int ani_set_id = atoi(tokens[0].c_str());

	LPANIMATION_SET s = new CAnimationSet();

	CAnimations* animations = CAnimations::GetInstance();

	for (size_t i = 1; i < tokens.size(); i++)
	{
		int ani_id = atoi(tokens[i].c_str());

		LPANIMATION ani = animations->Get(ani_id);
		s->push_back(ani);
	}

	CAnimationSets::GetInstance()->Add(ani_set_id, s);
}

void CIntroScene::_ParseSection_OBJECTS(string line) {
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return;

	int object_type = atoi(tokens[0].c_str());
	float x = stof(tokens[1].c_str());
	float y = stof(tokens[2].c_str());

	int ani_set_id = atoi(tokens[3].c_str());

	CAnimationSets* animation_sets = CAnimationSets::GetInstance();

	CBackgroundObject* obj = NULL;

	if (object_type == OBJECT_TYPE_MENU_BG)
		obj = new CBackgroundTile();
	else {
		if (object_type == OBJECT_TYPE_CURTAIN) {
			int h = atoi(tokens[4].c_str());
			obj = new CCurtain(h);
		}
	}

	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	obj->SetPosition(x, y);
	obj->SetAnimationSet(ani_set);
	objects.push_back(obj);
}

void CIntroScene::SetCursor(CTitleCursor* cursor) {
	this->cursor = cursor;
	objects.push_back(cursor);
}

void CIntroScene::Load() {
	ifstream f;
	f.open(sceneFilePath);

	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;

		if (line == "[TEXTURES]") { section = SCENE_SECTION_TEXTURES; continue; }
		if (line == "[SPRITES]") {
			section = SCENE_SECTION_SPRITES; continue;
		}
		if (line == "[ANIMATIONS]") {
			section = SCENE_SECTION_ANIMATIONS; continue;
		}
		if (line == "[ANIMATION_SETS]") {
			section = SCENE_SECTION_ANIMATION_SETS; continue;
		}
		if (line == "[OBJECTS]") {
			section = SCENE_SECTION_OBJECTS; continue;
		}
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		switch (section)
		{
		case SCENE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
		case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		}
	}

	f.close();
}

void CIntroScene::Update(DWORD dt) {
	for (size_t i = 0; i < objects.size(); i++)
		objects[i]->Update(dt);

	if (cursor == NULL)
		return;
}

void CIntroScene::Render() {
	for (size_t i = 0; i < objects.size(); i++)
		objects[i]->Render();
}

void CIntroScene::Unload() {
	for (size_t i = 0; i < objects.size(); i++)
		delete objects[i];

	objects.clear();
	cursor = NULL;
}

void CIntroSceneKeyHandler::OnKeyDown(int keyCode) {
	CTitleCursor* select = ((CIntroScene*)scene)->getCursor();

	if (select != NULL) {
		switch (keyCode) {
		case DIK_W:
		case DIK_S:
			if (select != NULL)
				select->ChangeState();
			break;
		case DIK_I:
			if (select->GetState() == CURSOR_SP) {
				CGame::GetInstance()->InitHUD();
				CGame::GetInstance()->SwitchScene(1);
			}
			break;
		}
	}
	else
		return;
}