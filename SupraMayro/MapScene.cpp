#include <iostream>
#include <fstream>

#include "MapScene.h"
#include "Utility.h"
#include "Textures.h"
#include "Sprites.h"
#include "Animations.h"

CMapScene::CMapScene(int id, LPCWSTR filePath) :CScene(id, filePath) {
	key_handler = new CMapSceneKeyHandler(this);
	mario = NULL;
	destination_scene = NULL;
	hud = NULL;
	gameover = NULL;
	IsGameOver = false;
}

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_TEXTURES 2
#define SCENE_SECTION_SPRITES 3
#define SCENE_SECTION_ANIMATIONS 4
#define SCENE_SECTION_ANIMATION_SETS 5
#define SCENE_SECTION_OBJECTS 6

#define OBJECT_TYPE_MARIO 0
#define OBJECT_TYPE_PATH 1
#define OBJECT_TYPE_BACKGROUND 2
#define OBJECT_TYPE_LEVEL 3
#define OBJECT_TYPE_OVERLAY 4
#define OBJECT_TYPE_HUD 100

#define MAX_SCENE_LINE 1024

using namespace std;

void CMapScene::_ParseSection_TEXTURES(string line)
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

void CMapScene::_ParseSection_SPRITES(string line)
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

void CMapScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return;

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i < tokens.size(); i += 2)
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i + 1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
}

void CMapScene::_ParseSection_ANIMATION_SETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return;

	int ani_set_id = atoi(tokens[0].c_str());

	LPANIMATION_SET s = new CAnimationSet();

	CAnimations* animations = CAnimations::GetInstance();

	for (int i = 1; i < tokens.size(); i++)
	{
		int ani_id = atoi(tokens[i].c_str());

		LPANIMATION ani = animations->Get(ani_id);
		s->push_back(ani);
	}

	CAnimationSets::GetInstance()->Add(ani_set_id, s);
}

void CMapScene::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return;

	int object_type = atoi(tokens[0].c_str());
	float x = atof(tokens[1].c_str());
	float y = atof(tokens[2].c_str());

	int ani_set_id = atoi(tokens[3].c_str());

	CAnimationSets* animation_sets = CAnimationSets::GetInstance();

	CGameObject* obj = NULL;
	CBackgroundObject* bg_obj = NULL;

	bool IsGameObject = true;

	switch (object_type) {
	case OBJECT_TYPE_MARIO:
		obj = new CMapMario();
		mario = (CMapMario*)obj;
		break;
	case OBJECT_TYPE_PATH:
		IsGameObject = false;
		bg_obj = new CBackgroundTile();
		break;
	case OBJECT_TYPE_BACKGROUND:
		obj = new CBlock();
		break;
	case OBJECT_TYPE_LEVEL: {
		int sid = atoi(tokens[4].c_str());
		obj = new CMapLevelBlock(sid);
		break;
	}
	case OBJECT_TYPE_OVERLAY: {
		IsGameObject = false;
		int l = atoi(tokens[4].c_str());
		int h = atoi(tokens[5].c_str());
		bg_obj = new CBackgroundObjectRect(l, h);
		break;
	}
	case OBJECT_TYPE_HUD:
		IsGameObject = false;
		bg_obj = new CHUD();
		hud = (CHUD*)bg_obj;
		break;
	}

	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	if (IsGameObject) {
		obj->SetPosition(x, y);
		obj->SetAnimationSet(ani_set);
		objects.push_back(obj);
	}
	else {
		bg_obj->SetPosition(x, y);
		bg_obj->SetAnimationSet(ani_set);
		bg_objects.push_back(bg_obj);
	}
}

void CMapScene::Load() {
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

	CTextures::GetInstance()->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));
}

void CMapScene::Update(DWORD dt) {
	vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 0; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt, &coObjects);
	}

	hud->Update(dt);

	if ((CGame::GetInstance()->GetLives() == 0) && (!IsGameOver)) {
		IsGameOver = true;
		CGameOver* game_over = new CGameOver();
		game_over->SetPosition(64, 64);
		gameover = (CGameOver*)game_over;
		bg_objects.push_back(game_over);
	}

	if (mario == NULL)
		return;
}

void CMapScene::Render() {
	for (int j = 0; j < bg_objects.size(); j++) {
		if (dynamic_cast<CGameOver*>(bg_objects.at(j)))
			continue;
		bg_objects[j]->Render();
	}
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->Render();
	}
	if (gameover != NULL)
		gameover->Render();
}

void CMapScene::Unload() {
	for (int i = 0; i < objects.size(); i++) {
		delete objects[i];
	}
	for (int j = 0; j < bg_objects.size(); j++) {
		delete bg_objects[j];
	}
	objects.clear();
	bg_objects.clear();
	mario = NULL;
	hud = NULL;
	gameover = NULL;
	IsGameOver = false;
}

void CMapSceneKeyHandler::OnKeyDown(int keyCode) {
	CMapMario* player = ((CMapScene*)scene)->GetPlayer();
	CGameOver* game_over = ((CMapScene*)scene)->GetGameOverInterface();

	if (((CMapScene*)scene)->GetGameStatus()){
		switch (keyCode) {
		case DIK_W:
		case DIK_S:
			game_over->ChangeOption();
			break;
		case DIK_K:
			game_over->EnforceOption();
			break;
		}
	}
	else {
		if (player->IsIdle()) {
			switch (keyCode) {
			case DIK_A:
				player->SetState(STATE_MOVE_LEFT);
				break;
			case DIK_W:
				player->SetState(STATE_MOVE_UP);
				break;
			case DIK_D:
				player->SetState(STATE_MOVE_RIGHT);
				break;
			case DIK_S:
				player->SetState(STATE_MOVE_DOWN);
				break;
			case DIK_1:
				player->SetPCForm(FORM_NORMAL);
				break;
			case DIK_2:
				player->SetPCForm(FORM_SUPER);
				break;
			case DIK_3:
				player->SetPCForm(FORM_RACCOON);
				break;
			case DIK_4:
				player->SetPCForm(FORM_FIRE);
				break;
			case DIK_K:
				int scene_id = ((CMapScene*)scene)->GetDestinationScene();
				if (scene_id != NULL) {
					CGame::GetInstance()->SetPrevForm(player->GetPCForm());
					CGame::GetInstance()->SwitchScene(scene_id);
				}
				break;
			}
		}
	}
}