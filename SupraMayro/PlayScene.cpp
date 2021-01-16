#include <iostream>
#include <fstream>

#include "PlayScene.h"
#include "Utility.h"
#include "Textures.h"
#include "Sprites.h"
#include "Animations.h"

CPlayScene::CPlayScene(int id, LPCWSTR filePath, float x, float X, float y, float Y, int xOnMap, int yOnMap, int scroll) :CScene(id, filePath) {
	key_handler = new CPlaySceneKeyHandler(this);
	player = NULL;
	tail = NULL;
	hud = NULL;
	minX = x;
	maxX = X;
	minY = y;
	maxY = Y;
	this->xOnMap = xOnMap;
	this->yOnMap = yOnMap;

	NumOfFireballs = 0;

	if (scroll == 0)
		AutoScroll = false;
	else
		AutoScroll = true;

	grid = new CGrid(maxX - minX, maxY - minY, minX, minY);
	timer_start = course_end = 0;
	timer = 0;
	CourseCompleted = false;
}

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_TEXTURES 2
#define SCENE_SECTION_SPRITES 3
#define SCENE_SECTION_ANIMATIONS 4
#define SCENE_SECTION_ANIMATION_SETS 5
#define SCENE_SECTION_OBJECTS 6
#define SCENE_SECTION_CELLS 7

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
#define OBJECT_TYPE_GOOMBA 11
#define OBJECT_TYPE_KOOPA 12
#define OBJECT_TYPE_BGHEIGHT 13
#define OBJECT_TYPE_BGRECT 14
#define OBJECT_TYPE_COIN 15
#define OBJECT_TYPE_POWERUP 16
#define OBJECT_TYPE_PIRANHA_PLANT 17
#define OBJECT_TYPE_PSWITCH 18
#define OBJECT_TYPE_PORTAL 19
#define OBJECT_TYPE_ROULETTE 20
#define OBJECT_TYPE_COINBLOCK 21
#define OBJECT_TYPE_LIFT 22
#define OBJECT_TYPE_BBRO 23
#define OBJECT_TYPE_HUD 100

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
	for (size_t i = 1; i < tokens.size(); i += 2)
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

	for (size_t i = 1; i < tokens.size(); i++)
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
	float x = stof(tokens[1].c_str());
	float y = stof(tokens[2].c_str());

	int ani_set_id = atoi(tokens[3].c_str());

	CAnimationSets* animation_sets = CAnimationSets::GetInstance();

	CGameObject* obj = NULL;
	CBackgroundObject* bg_obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_MARIO:
		obj = new CMario(x, y);
		player = (CMario*)obj;
		player->SetPosition(x, y);
		player->GetInitBoundaries();
		player->SetAnimationSet(animation_sets->Get(ani_set_id));

		tail = new CTail();
		break;
	case OBJECT_TYPE_HUD: {
		bg_obj = new CHUD();
		hud = (CHUD*)bg_obj;
		hud->SetAnimationSet(animation_sets->Get(ani_set_id));
		break;
	}
	}
}

void CPlayScene::_ParseSection_CELLS(string line, int row, int column) {
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return;

	int object_type = atoi(tokens[0].c_str());
	float x = stof(tokens[1].c_str());
	float y = stof(tokens[2].c_str());

	int ani_set_id = atoi(tokens[3].c_str());

	CAnimationSets* animation_sets = CAnimationSets::GetInstance();

	CGameObject* obj = NULL;
	CBackgroundObject* bg_obj = NULL;

	bool IsGameObject = true;

	switch (object_type) {
	case OBJECT_TYPE_BLOCK:
		obj = new CBlock();
		break;
	case OBJECT_TYPE_GBLOCK:
		obj = new CGoldBlock();
		break;
	case OBJECT_TYPE_PBLOCK: {
		int t = atoi(tokens[4].c_str());
		obj = new CPBlock(t);
		break;
	}
	case OBJECT_TYPE_PIPE:
	{
		int h = atoi(tokens[4].c_str());
		int t = atoi(tokens[5].c_str());
		int w = atoi(tokens[6].c_str());
		obj = new CPipe(h, t, w);
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
		for (size_t i = 4; i < tokens.size(); i++) {
			d.push_back(atoi(tokens[i].c_str()));
		}
		bg_obj = new CShrub(d);
		break;
	}
	case OBJECT_TYPE_GOOMBA: {
		int x = atoi(tokens[4].c_str());
		int X = atoi(tokens[5].c_str());
		int p = atoi(tokens[6].c_str());
		if (p == 0)
			obj = new CGoomba(x, X, false);
		else
			obj = new CGoomba(x, X, true);
		break;
	}
	case OBJECT_TYPE_KOOPA: {
		int t = atoi(tokens[4].c_str());
		switch (t) {
		case 0: {
			int min_x = atoi(tokens[5].c_str());
			int max_x = atoi(tokens[6].c_str());
			obj = new CKoopa(min_x, max_x);
			break;
		}
		case 1: {
			int min_x = atoi(tokens[5].c_str());
			int max_x = atoi(tokens[6].c_str());
			int px = atoi(tokens[7].c_str());
			int pX = atoi(tokens[8].c_str());
			obj = new CKoopa(min_x, max_x, px, pX);
			break;
		}
		case 2: {
			int min_y = atoi(tokens[5].c_str());
			int max_y = atoi(tokens[6].c_str());
			obj = new CKoopa((int)x, min_y, max_y);
			break;
		}
		}
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
	case OBJECT_TYPE_COIN: {
		obj = new CCoin(0);
		break;
	}
	case OBJECT_TYPE_PIRANHA_PLANT: {
		int t = atoi(tokens[4].c_str());
		obj = new CPiranhaPlant(t);
		break;
	}
	case OBJECT_TYPE_PSWITCH: {
		obj = new CPSwitch();
		break;
	}
	case OBJECT_TYPE_PORTAL: {
		int w = atoi(tokens[4].c_str());
		int h = atoi(tokens[5].c_str());
		int sid = atoi(tokens[6].c_str());
		int nx = atoi(tokens[7].c_str());
		int ny = atoi(tokens[8].c_str());
		int dir = atoi(tokens[9].c_str());
		obj = new CPortal(w, h, sid, nx, ny, dir);
		break;
	}
	case OBJECT_TYPE_ROULETTE: {
		obj = new CRoulette();
		break;
	}
	case OBJECT_TYPE_COINBLOCK: {
		int u = atoi(tokens[4].c_str());
		obj = new CCoinBlock(u);
		break;
	}
	case OBJECT_TYPE_LIFT: {
		obj = new CLift();
		break;
	}
	case OBJECT_TYPE_BBRO: {
		int min_x = atoi(tokens[4].c_str());
		int max_x = atoi(tokens[5].c_str());
		obj = new CBoomerangBro(min_x, max_x);
		break;
	}
	}

	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	if (IsGameObject) {
		obj->SetPosition(x, y);
		obj->GetInitBoundaries();
		obj->SetAnimationSet(ani_set);
		grid->AddToGrid(row, column, obj);
	}
	else {
		bg_obj->SetPosition(x, y);
		bg_obj->SetAnimationSet(ani_set);
		grid->AddToGrid(row, column, bg_obj);
	}
}

void CPlayScene::Load()
{
	ifstream f;
	f.open(sceneFilePath);
	
	int section = SCENE_SECTION_UNKNOWN;

	int cellRow = 0;
	int cellColumn = 0;

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
		if (line == "[CELLS]") {
			section = SCENE_SECTION_CELLS; continue;
		}
		if (line[0] == 'C') {
			vector<string> tokens = split(line);
			cellRow = atoi(tokens[1].c_str());
			cellColumn = atoi(tokens[2].c_str());
			continue;
		}
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		switch (section)
		{
		case SCENE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
		case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		case SCENE_SECTION_CELLS: _ParseSection_CELLS(line, cellRow, cellColumn); break;
		}
	}

	f.close();

	if (CGame::GetInstance()->GetTime() == 0)
		CGame::GetInstance()->InitTimer();

	CTextures::GetInstance()->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));
}

void CPlayScene::PushObject(LPGAMEOBJECT obj) {
	if (grid->LocateCellByObject(obj) != NULL)
		(grid->LocateCellByObject(obj))->AddToCell(obj);
}

void CPlayScene::Replace(int i, LPGAMEOBJECT newObj) {
	(grid->LocateCellByObject(newObj))->Replace(i, newObj);
}

void CPlayScene::RemoveObject(LPGAMEOBJECT obj) {
	if (grid->LocateCellByObject(obj) != NULL)
		(grid->LocateCellByObject(obj))->DeleteObject(obj);
}

void CPlayScene::Update(DWORD dt)
{
	if (player == NULL)
		return;

	float fx, fy;
	player->GetPosition(fx, fy);

	if (fx <= minX + 1)
		player->SetPosition(minX + 1, fy);

	if (fy <= minY + 1)
		player->SetPosition(fx, minY + 1);

	if (AutoScroll) {
		float cx, cy; CGame::GetInstance()->GetCamPos(cx, cy = 0);
		int scrnw = CGame::GetInstance()->GetScreenWidth();
		
		int MW = 0;
		if (player->GetPCForm() == MARIO_FORM_NORMAL)
			MW = MARIO_NORMAL_BBOX_WIDTH;
		else
			MW = MARIO_SUPER_BBOX_WIDTH;

		if (fx <= cx)
			player->SetPosition(cx, fy);
		if (fx >= cx + scrnw - MW * 2)
			player->SetPosition(cx + scrnw - MW * 2, fy);
	}

	LPCELL current_main_cell = grid->LocateCellByObject(player);

	if (current_main_cell != NULL) {
		if (player->GetState() != MARIO_STATE_DEATH){
			vector<LPCELL> active_cells;
			grid->GetAdjacentCells(current_main_cell, active_cells);

			for (size_t i = 0; i < active_cells.size(); i++) {
				for (UINT j = 0; j < active_cells[i]->GetObjects().size(); j++) {
					if ((dynamic_cast<CGoomba*>(active_cells[i]->GetObjects()[j]))
						|| (dynamic_cast<CKoopa*>(active_cells[i]->GetObjects()[j]))
						|| (dynamic_cast<CFireball*>(active_cells[i]->GetObjects()[j]))
						|| (dynamic_cast<CPowerUp*>(active_cells[i]->GetObjects()[j]))
						|| (dynamic_cast<CLift*>(active_cells[i]->GetObjects()[j]))
						|| (dynamic_cast<CBoomerangBro*>(active_cells[i]->GetObjects()[j]))
						|| (dynamic_cast<CBoomerang*>(active_cells[i]->GetObjects()[j]))) {
						bool result = grid->CheckObjectIntegrity(active_cells[i]->GetObjects()[j], active_cells[i]);
						if (!result) {
							if (dynamic_cast<CFireball*>(active_cells[i]->GetObjects()[j])) {
								if (dynamic_cast<CFireball*>(active_cells[i]->GetObjects()[j])->GetType() == FIREBALL_MARIO)
									NumOfFireballs--;
							}
							active_cells[i]->DeleteObject(active_cells[i]->GetObjects()[j]);
						}
					}
				}
			}

			vector<LPGAMEOBJECT> coObjects;
			for (size_t i = 0; i < active_cells.size(); i++) {
				for (size_t j = 0; j < active_cells[i]->GetObjects().size(); j++) {
					coObjects.push_back(active_cells[i]->GetObjects()[j]);
				}
			}
			coObjects.push_back(player);
			if (player->GetPCForm() == MARIO_FORM_RACCOON) {
				coObjects.push_back(tail);
				tail->Update(dt, &coObjects);
			}

			for (size_t i = 0; i < active_cells.size(); i++) {
				for (UINT j = 0; j < active_cells[i]->GetObjects().size(); j++) {
					if ((player->GetState() == MARIO_STATE_EMERGING_UP) || (player->GetState() == MARIO_STATE_EMERGING_DOWN)) {
						if ((dynamic_cast<CGoomba*>(active_cells[i]->GetObjects()[j]))
							|| (dynamic_cast<CKoopa*>(active_cells[i]->GetObjects()[j]))
							|| (dynamic_cast<CBoomerangBro*>(active_cells[i]->GetObjects()[j])))
							continue;
					}

					active_cells[i]->GetObjects()[j]->Update(dt, &coObjects);
				}
			}

			player->Update(dt, &coObjects);

			if (hud == NULL)
				return;
			else
				hud->Update(dt);

			if (GetTickCount() - timer_start > TIC) {
				if (timer == 1) {
					CGame::GetInstance()->TimerTic();
					timer_start = 0;
				}
				StartTimer();
			}

			if ((CGame::GetInstance()->GetTime() == 0) && (!CourseCompleted)) {
				timer = 0;
				timer_start = 0;
				player->SetPCForm(MARIO_FORM_NORMAL);
				player->SetState(MARIO_STATE_DEATH);
			}

			if (!AutoScroll) {
				float cx, cy;
				player->GetPosition(cx, cy);

				cx -= CGame::GetInstance()->GetScreenWidth() / 2;
				cy -= CGame::GetInstance()->GetScreenHeight() / 4;

				if (cx <= minX)
					CGame::GetInstance()->SetCamX(minX);
				else {
					if (cx >= maxX - CGame::GetInstance()->GetScreenWidth() + 16.0f)
						CGame::GetInstance()->SetCamX(maxX - CGame::GetInstance()->GetScreenWidth() + 16.0f);
					else
						CGame::GetInstance()->SetCamX(cx);
				}

				if (cy <= minY)
					CGame::GetInstance()->SetCamY(minY);
				else {
					if (cy >= maxY - CGame::GetInstance()->GetScreenHeight() + 68.0f)
						CGame::GetInstance()->SetCamY(maxY - CGame::GetInstance()->GetScreenHeight() + 68.0f);
					else
						CGame::GetInstance()->SetCamY(cy);
				}
			}
			else {
				float cx, cy;
				CGame::GetInstance()->GetCamPos(cx, cy);

				cx += CAMERA_SCROLL_SPEED;
				if (cx >= maxX - CGame::GetInstance()->GetScreenWidth() + 16.0f)
					CGame::GetInstance()->SetCamPos(maxX - CGame::GetInstance()->GetScreenWidth() + 16.0f, 64.0f);
				else
					CGame::GetInstance()->SetCamPos(cx, 64.0f);
			}

			float Cx, Cy;
			CGame::GetInstance()->GetCamPos(Cx, Cy);

			hud->SetPosition(Cx, Cy + 188.0f);

			if (CourseCompleted) {
				timer = 0;
				timer_start = 0;
				if (GetTickCount() - course_end > COURSE_END_DELAY) {
					CGame::GetInstance()->TimerTic();
					if (CGame::GetInstance()->GetTime() != 0)
						CGame::GetInstance()->AddScore(50);
					else
						CGame::GetInstance()->SwitchScene(1);
				}
			}
		}
		else
			player->Update(dt);
	}
	else {
		float fx, fy;
		player->GetPosition(fx, fy);

		if (fy >= maxY) {
			if (!CourseCompleted) {
				CGame::GetInstance()->SetPrevForm(MARIO_FORM_NORMAL);
				CGame::GetInstance()->LoseLife();
				CGame::GetInstance()->InitTimer();
				CGame::GetInstance()->SwitchScene(1);
			}
		}
	}
}

void CPlayScene::Render()
{
	LPCELL current_main_cell = grid->LocateCellByObject(player);
	
	if (current_main_cell == NULL) return;

	vector<LPCELL> active_cells;
	grid->GetAdjacentCells(current_main_cell, active_cells);

	vector<LPGAMEOBJECT> pipes, blocks, lifts;

	for (size_t i = 0; i < active_cells.size(); i++) {
		for (size_t j = 0; j < active_cells[i]->GetBackgroundObjects().size(); j++)
			active_cells[i]->GetBackgroundObjects()[j]->Render();
	}

	for (size_t i = 0; i < active_cells.size(); i++) {
		for (size_t j = 0; j < active_cells[i]->GetObjects().size(); j++) {
			if (dynamic_cast<CBackgroundPlatform*>(active_cells[i]->GetObjects()[j]))
				active_cells[i]->GetObjects()[j]->Render();
			else
				continue;
		}
	}

	for (size_t i = 0; i < active_cells.size(); i++) {
		for (size_t j = 0; j < active_cells[i]->GetObjects().size(); j++) {
			if (dynamic_cast<CBackgroundPlatform*>(active_cells[i]->GetObjects()[j]))
				continue;

			if (dynamic_cast<CPipe*>(active_cells[i]->GetObjects()[j])) {
				pipes.push_back(active_cells[i]->GetObjects()[j]);
				continue;
			}
			else {
				if (dynamic_cast<CBlock*>(active_cells[i]->GetObjects()[j])) {
					blocks.push_back(active_cells[i]->GetObjects()[j]);
					continue;
				}
				else {
					if (dynamic_cast<CLift*>(active_cells[i]->GetObjects()[j])) {
						lifts.push_back(active_cells[i]->GetObjects()[j]);
						continue;
					}
					else
						active_cells[i]->GetObjects()[j]->Render();
				}
			}
		}
	}
	
	player->Render();
	//tail->Render();

	if (pipes.size() != 0) {
		for (size_t i = 0; i < pipes.size(); i++)
			pipes[i]->Render();
	}

	if (blocks.size() != 0) {
		for (size_t i = 0; i < blocks.size(); i++)
			blocks[i]->Render();
	}

	if (lifts.size() != 0) {
		for (size_t i = 0; i < lifts.size(); i++)
			lifts[i]->Render();
	}

	hud->Render();
}

void CPlayScene::Unload()
{
	grid->ClearGrid();
	player = NULL;
	tail = NULL;
	hud = NULL;
	CourseCompleted = false;
	NumOfFireballs = 0;
	timer = 0;
	timer_start = course_end = 0;
}

void CPlayScene::EndCourse() {
	CourseCompleted = true;
	player->SetSpeed(0, 0);
	timer = 0;
	timer_start = 0;
	course_end = GetTickCount();
	CGame::GetInstance()->SetPrevForm(player->GetPCForm());
	CGame::GetInstance()->SetNewPos(xOnMap, yOnMap);
}

void CPlaySceneKeyHandler::OnKeyDown(int KeyCode)
{
	CMario* mario = ((CPlayScene*)scene)->getPlayer();
	switch (KeyCode) {
	case DIK_J: {
		if (mario->GetPCForm() == MARIO_FORM_RACCOON
			|| (mario->GetPCForm() == MARIO_FORM_FIRE && (((CPlayScene*)scene)->GetFireballs() < 1))) {
			if (!mario->GetAttackStatus()) {
				mario->SetState(MARIO_STATE_ATTACK);
			}
		}
		mario->StartSprint();
		break;
	}
	case DIK_K:
		if (mario->GetGravStatus())
			mario->SetState(MARIO_STATE_JUMP);
		else {
			if (mario->GetPCForm() == MARIO_FORM_RACCOON) {
				mario->SetState(MARIO_STATE_FLAP);
			}
		}
		break;
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
	switch (KeyCode) {
	case DIK_S: {
		if ((mario->GetState() != MARIO_STATE_WARPING_UP) && (mario->GetState() != MARIO_STATE_WARPING_DOWN)) {
			mario->SetState(MARIO_STATE_IDLE);
			if (mario->GetPCForm() != MARIO_FORM_NORMAL)
				mario->ModY(MARIO_CROUCH_BBOX_HEIGHT - MARIO_SUPER_BBOX_HEIGHT); // w/o this Mario will clip through platforms after standing up
		}
		break;
	}
	case DIK_J:
		mario->SetCarryStatus(false);
		mario->SetSprintCharge(false);
		break;	
	case DIK_K:
		mario->SetJumpCharge(false);
		break;
	case DIK_W:
		mario->SetWButtonStatus(false);
		break;
	}
}

void CPlaySceneKeyHandler::KeyState(BYTE* states)
{
	CGame* game = CGame::GetInstance();
	CMario* mario = ((CPlayScene*)scene)->getPlayer();

	if (mario->GetState() == MARIO_STATE_DEATH || mario->GetState() == MARIO_STATE_CROUCH ||
		mario->GetState() == MARIO_STATE_WARPING_UP || mario->GetState() == MARIO_STATE_WARPING_DOWN ||
		mario->GetState() == MARIO_STATE_EMERGING_UP || mario->GetState()==MARIO_STATE_EMERGING_DOWN
		|| ((CPlayScene*)scene)->GetCourseStatus())
		return;

	if (game->IsKeyDown(DIK_J)) {
		mario->SetCarryStatus(true);
		if ((!mario->GetAttackStatus()) || (mario->GetPCForm() == MARIO_FORM_NORMAL) || (mario->GetPCForm() == MARIO_FORM_SUPER)) {
			mario->ToggleSprint();
			mario->SetSprintCharge(true);
		}
	}

	if (game->IsKeyDown(DIK_W)) {
		if (mario->GetState() == MARIO_STATE_WARPING_UP)
			return;
		else
			mario->SetWButtonStatus(true);
	}

	if (game->IsKeyDown(DIK_K)) {
		float vy, vx = 0; mario->GetSpeed(vx, vy);
		if (vy < 0)
			mario->SetJumpCharge(true);
	}

	if (game->IsKeyDown(DIK_D)) {
		mario->SetState(MARIO_STATE_WALK_RIGHT);
	}
	else {
		if (game->IsKeyDown(DIK_A)) {
			mario->SetState(MARIO_STATE_WALK_LEFT);
		}
		else {
			if (game->IsKeyDown(DIK_S)) {
				mario->SetState(MARIO_STATE_CROUCH);
				if (mario->GetPCForm() != MARIO_FORM_NORMAL)
					mario->ModY(MARIO_SUPER_BBOX_HEIGHT - MARIO_CROUCH_BBOX_HEIGHT);
			}
			else
				mario->SetState(MARIO_STATE_IDLE);
		}
	}
}