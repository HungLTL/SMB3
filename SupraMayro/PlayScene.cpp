#include <iostream>
#include <fstream>

#include "PlayScene.h"
#include "Utility.h"
#include "Textures.h"
#include "Sprites.h"
#include "Animations.h"

CPlayScene::CPlayScene(int id, LPCWSTR filePath) :CScene(id, filePath) {
	key_handler = new CPlaySceneKeyHandler(this);
}

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_TEXTURES 2
#define SCENE_SECTION_SPRITES 3
#define SCENE_SECTION_ANIMATIONS 4
#define SCENE_SECTION_ANIMATION_SETS 5
#define SCENE_SECTION_OBJECTS 6

#define OBJECT_TYPE_MARIO 0
#define OBJECT_TYPE_BLOCK 1
#define OBJECT_TYPE_GBLOCK 2
#define OBJECT_TYPE_PBLOCK 3
#define OBJECT_TYPE_PIPE 4
#define OBJECT_TYPE_WPLATFORM 5
#define OBJECT_TYPE_BGPLATFORM 6
#define OBJECT_TYPE_BGTILE 7
#define OBJECT_TYPE_CLOUD 8
#define OBJECT_TYPE_SHRUB 9
#define OBJECT_TYPE_FIREBALL 10
#define OBJECT_TYPE_GOOMBA 11
#define OBJECT_TYPE_KOOPA 12
#define OBJECT_TYPE_BGHEIGHT 13
#define OBJECT_TYPE_BGRECT 14

#define MAX_SCENE_LINE 1024

using namespace std;

void CPlayScene::_ParseSection_TEXTURES(string line)
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

void CPlayScene::_ParseSection_SPRITES(string line)
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

void CPlayScene::_ParseSection_ANIMATIONS(string line)
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

void CPlayScene::_ParseSection_ANIMATION_SETS(string line)
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

void CPlayScene::_ParseSection_OBJECTS(string line)
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

	switch (object_type)
	{
	case OBJECT_TYPE_MARIO:
		obj = new CMario(x, y);
		player = (CMario*)obj;
		break;
	case OBJECT_TYPE_BLOCK:
		obj = new CBlock();
		break;
	case OBJECT_TYPE_GBLOCK:
		obj = new CGoldBlock();
		break;
	case OBJECT_TYPE_PBLOCK:
		obj = new CPBlock();
		break;
	case OBJECT_TYPE_PIPE:
	{
		int h = atoi(tokens[4].c_str());
		obj = new CPipe(h);
		break;
	}
	case OBJECT_TYPE_WPLATFORM:
	{
		int h = atoi(tokens[4].c_str());
		int l = atoi(tokens[5].c_str());
		obj = new CWoodPlatform(l, h);
		break;
	}
	case OBJECT_TYPE_BGPLATFORM:
	{
		int h = atoi(tokens[4].c_str());
		int l = atoi(tokens[5].c_str());
		int c = atoi(tokens[6].c_str());
		obj = new CBackgroundPlatform(l, h, c);
		break;
	}
	case OBJECT_TYPE_BGTILE:
		IsGameObject = false;
		bg_obj = new CBackgroundTile();
		break;
	case OBJECT_TYPE_CLOUD:
	{
		IsGameObject = false;
		int l = atoi(tokens[4].c_str());
		bg_obj = new CCloud(l);
		break;
	}
	case OBJECT_TYPE_SHRUB:
	{
		IsGameObject = false;
		vector<int> d;
		for (int i = 4; i < tokens.size(); i++) {
			d.push_back(atoi(tokens[i].c_str()));
		}
		bg_obj = new CShrub(d);
		break;
	}
	case OBJECT_TYPE_GOOMBA: {
		int x = atoi(tokens[4].c_str());
		int X = atoi(tokens[5].c_str());
		obj = new CGoomba(x, X);
		break;
	}
	case OBJECT_TYPE_KOOPA: {
		int x = atoi(tokens[4].c_str());
		int X = atoi(tokens[5].c_str());
		obj = new CKoopa(x, X);
		break;
	}
	case OBJECT_TYPE_BGHEIGHT: {
		IsGameObject = false;
		int h = atoi(tokens[4].c_str());
		bg_obj = new CBackgroundObjectHeight(h);
		break;
	}
	case OBJECT_TYPE_BGRECT: {
		IsGameObject = false;
		int h = atoi(tokens[4].c_str());
		int l = atoi(tokens[5].c_str());
		bg_obj = new CBackgroundObjectRect(l, h);
		break;
	}
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

void CPlayScene::Load()
{
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

void CPlayScene::Update(DWORD dt)
{
	vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 1; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt, &coObjects);
	}

	if (player == NULL)
		return;

	float cx, cy;
	player->GetPosition(cx, cy);

	CGame* game = CGame::GetInstance();
	cx -= game->GetScreenWidth() / 2;
	cy -= game->GetScreenHeight() / 2;

	CGame::GetInstance()->SetCamPos(cx - 64.0f, cy - 192.0f);
}

void CPlayScene::Render()
{
	for (int j = 0; j < bg_objects.size(); j++) {
		bg_objects[j]->Render();
	}
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->Render();
	}
}

void CPlayScene::Unload()
{
	for (int i = 0; i < objects.size(); i++) {
		delete objects[i];
	}
	for (int j = 0; j < bg_objects.size(); j++) {
		delete bg_objects[j];
	}
	objects.clear();
	bg_objects.clear();
	player = NULL;
}

void CPlaySceneKeyHandler::OnKeyDown(int KeyCode)
{
	CMario* mario = ((CPlayScene*)scene)->getPlayer();
	int f = mario->GetPCForm();
	switch (KeyCode) {
	case DIK_K:
	{
		mario->SetState(MARIO_STATE_JUMP);
		break;
	}
	case DIK_J: {
		if (f == MARIO_FORM_RACCOON || f == MARIO_FORM_FIRE) {
			mario->SetState(MARIO_STATE_ATTACK);
			if (f == MARIO_FORM_FIRE) {
				if (mario->GetAttackStatus()) {
					if (mario->GetAttackTime() == 0) {
						float fx, fy;
						mario->GetPosition(fx, fy);

						CFireball* fireball;
						if (mario->GetNX() > 0) {
							fireball = new CFireball(true);
							fireball->SetPosition(fx + 11, fy + 15);
						}
						else {
							fireball = new CFireball(false);
							fireball->SetPosition(fx, fy + 15);
						}

						CAnimationSets* animation_sets = CAnimationSets::GetInstance();
						LPANIMATION_SET ani_set = animation_sets->Get(OBJECT_TYPE_FIREBALL);
						fireball->SetAnimationSet(ani_set);
						vector<LPGAMEOBJECT> vec = (((CPlayScene*)scene)->getObjects());
						vec.push_back(fireball);
						((CPlayScene*)scene)->updateObjects(vec);
					}
				}
			}
		}
		break;
	}
	case DIK_1:
		mario->SetPCForm(MARIO_FORM_NORMAL);
		break;
	case DIK_2:
		mario->SetPCForm(MARIO_FORM_SUPER);
		break;
	case DIK_3:
		mario->SetPCForm(MARIO_FORM_RACCOON);
		break;
	case DIK_4:
		mario->SetPCForm(MARIO_FORM_FIRE);
		break;
	case DIK_R:
		mario->SetState(MARIO_STATE_DEATH);
		break;
	}
}

void CPlaySceneKeyHandler::OnKeyUp(int KeyCode) {
	CMario* mario = ((CPlayScene*)scene)->getPlayer();
	int f = mario->GetPCForm();
	if (KeyCode == DIK_S) {
		if (f != MARIO_FORM_NORMAL) {
			mario->SetState(MARIO_STATE_IDLE);
			float fx, fy;
			mario->GetPosition(fx, fy);
			mario->SetPosition(fx, fy - 9); // w/o this Mario will clip through platforms after standing up
		}
	}
	if (KeyCode == DIK_J) {
		mario->SetCarryStatus(false);
	}
}

void CPlaySceneKeyHandler::KeyState(BYTE* states)
{
	CGame* game = CGame::GetInstance();
	CMario* mario = ((CPlayScene*)scene)->getPlayer();

	if (mario->GetState() == MARIO_STATE_DEATH || mario->GetState() == MARIO_STATE_CROUCH)
		return;
	if (game->IsKeyDown(DIK_J))
		mario->SetCarryStatus(true);
	if (game->IsKeyDown(DIK_K)) {
		if (!mario->GetGravStatus()) {
			if (mario->GetPCForm() == MARIO_FORM_RACCOON) {
				mario->SetState(MARIO_STATE_FLY);
			}
		}
		else
			return;
	}
	if (game->IsKeyDown(DIK_D)) {
		mario->SetState(MARIO_STATE_WALK_RIGHT);
		if (game->IsKeyDown(DIK_J)) {
			if (!mario->GetAttackStatus())
				mario->SetState(MARIO_STATE_SPRINT_RIGHT);
		}
		if (game->IsKeyDown(DIK_K)) {
			if (!mario->GetGravStatus()) {
				if (mario->GetPCForm() == MARIO_FORM_RACCOON)
					mario->SetState(MARIO_STATE_FLY);
			}
			else
				return;
		}
	}
	else {
		if (game->IsKeyDown(DIK_A)) {
			mario->SetState(MARIO_STATE_WALK_LEFT);
			if (game->IsKeyDown(DIK_J)) {
				if (!mario->GetAttackStatus())
					mario->SetState(MARIO_STATE_SPRINT_LEFT);
			}
			if (game->IsKeyDown(DIK_K)) {
				if (!mario->GetGravStatus()) {
					if (mario->GetPCForm() == MARIO_FORM_RACCOON)
						mario->SetState(MARIO_STATE_FLY);
				}
				else
					return;
			}
		}
		else {
			if (game->IsKeyDown(DIK_S)) {
				if (mario->GetPCForm() != MARIO_FORM_NORMAL) {
					mario->SetState(MARIO_STATE_CROUCH);
					float fx, fy;
					mario->GetPosition(fx, fy);
					mario->SetPosition(fx, fy + 9); // optional: this will remove the "bouncy effect" from the change of bbox position
				}
			}
			else
				mario->SetState(MARIO_STATE_IDLE);
		}
	}
}