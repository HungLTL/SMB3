#include <iostream>
#include <fstream>

#include "Game.h"
#include "Utility.h"

#include "HUD.h"

#include "PlayScene.h"
#include "IntroScene.h"
#include "MapScene.h"

CGame* CGame::__instance = NULL;
int prevState = NULL;
int prevForm = NULL;
int newX = NULL;
int newY = NULL;

#define SCENE_TYPE_INTRO 0
#define SCENE_TYPE_MAP 1
#define SCENE_TYPE_GAME 2

void CGame::Init(HWND hWnd)
{
	LPDIRECT3D9 d3d = Direct3DCreate9(D3D_SDK_VERSION);
	this->hWnd = hWnd;

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;

	RECT r;
	GetClientRect(hWnd, &r);

	d3dpp.BackBufferHeight = r.bottom + 1;
	d3dpp.BackBufferWidth = r.right + 1;

	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddv);

	if (d3ddv == NULL)
		return;

	d3ddv->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);

	D3DXCreateSprite(d3ddv, &spriteHandler);
}

void CGame::Draw(float x, float y, LPDIRECT3DTEXTURE9 texture, int left, int top, int right, int bottom, int alpha)
{
	D3DXVECTOR3 p(x - cam_x, y - cam_y, 0);
	RECT r;
	r.left = left;
	r.top = top;
	r.right = right;
	r.bottom = bottom;
	spriteHandler->Draw(texture, &r, NULL, &p, D3DCOLOR_ARGB(alpha, 255, 255, 255));
}

int CGame::IsKeyDown(int KeyCode)
{
	return (keyStates[KeyCode] & 0x80) > 0;
}

void CGame::InitKeyboard()
{
	HRESULT
		hr = DirectInput8Create
		(
			(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			DIRECTINPUT_VERSION,
			IID_IDirectInput8, (VOID**)&di, NULL
		);

	hr = di->CreateDevice(GUID_SysKeyboard, &didv, NULL);

	hr = didv->SetDataFormat(&c_dfDIKeyboard);
	hr = didv->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	DIPROPDWORD dipdw;

	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = KEYBOARD_BUFFER_SIZE;

	hr = didv->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	hr = didv->Acquire();
}

void CGame::ProcessKeyboard()
{
	HRESULT hr;

	hr = didv->GetDeviceState(sizeof(keyStates), keyStates);
	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			HRESULT h = didv->Acquire();
		}
		else
		{
			return;
		}
	}

	keyHandler->KeyState((BYTE*)&keyStates);

	DWORD dwElements = KEYBOARD_BUFFER_SIZE;
	hr = didv->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), keyEvents, &dwElements, 0);

	for (DWORD i = 0; i < dwElements; i++)
	{
		int KeyCode = keyEvents[i].dwOfs;
		int KeyState = keyEvents[i].dwData;
		if ((KeyState & 0x80) > 0)
			keyHandler->OnKeyDown(KeyCode);
		else
			keyHandler->OnKeyUp(KeyCode);
	}
}

CGame::~CGame()
{
	if (spriteHandler != NULL) spriteHandler->Release();
	if (backBuffer != NULL) backBuffer->Release();
	if (d3ddv != NULL) d3ddv->Release();
	if (d3d != NULL) d3d->Release();
}

void CGame::ResetCards() {
	cards.clear();
	cards = vector<int>(NUM_OF_CARDS, ANI_HUD_EMPTY);
}

void CGame::InitHUD() {
	lives = 4;
	score = time = coins = 0;
	ResetCards();
}

void CGame::AddCoin() {
	coins++;
	if (coins == 100) {
		AddLife();
		coins = 0;
	}
}

void CGame::AddCard(int value) {
	for (int i = 0; i < NUM_OF_CARDS; i++) {
		if (cards[i] == ANI_HUD_EMPTY) {
			cards[i] = value;
			break;
		}
	}
}

void CGame::SweptAABB(
	float ml, float mt, float mr, float mb,
	float dx, float dy,
	float sl, float st, float sr, float sb,
	float& t, float& nx, float& ny)
{

	float dx_entry, dx_exit, tx_entry, tx_exit;
	float dy_entry, dy_exit, ty_entry, ty_exit;

	float t_entry;
	float t_exit;

	t = -1.0f;
	nx = ny = 0;

	float bl = dx > 0 ? ml : ml + dx;
	float bt = dy > 0 ? mt : mt + dy;
	float br = dx > 0 ? mr + dx : mr;
	float bb = dy > 0 ? mb + dy : mb;

	if (br < sl || bl > sr || bb < st || bt > sb) return;

	if (dx == 0 && dy == 0) return;

	if (dx > 0)
	{
		dx_entry = sl - mr;
		dx_exit = sr - ml;
	}
	else if (dx < 0)
	{
		dx_entry = sr - ml;
		dx_exit = sl - mr;
	}

	if (dy > 0)
	{
		dy_entry = st - mb;
		dy_exit = sb - mt;
	}
	else if (dy < 0)
	{
		dy_entry = sb - mt;
		dy_exit = st - mb;
	}

	if (dx == 0)
	{
		tx_entry = -999999.0f;
		tx_exit = 999999.0f;
	}
	else
	{
		tx_entry = dx_entry / dx;
		tx_exit = dx_exit / dx;
	}

	if (dy == 0)
	{
		ty_entry = -99999.0f;
		ty_exit = 99999.0f;
	}
	else
	{
		ty_entry = dy_entry / dy;
		ty_exit = dy_exit / dy;
	}

	if ((tx_entry < 0.0f && ty_entry < 0.0f) || tx_entry > 1.0f || ty_entry > 1.0f ) return;

	t_entry = max(tx_entry, ty_entry);
	t_exit = min(tx_exit, ty_exit);

	if (t_entry > t_exit) return;

	t = t_entry;

	if (tx_entry > ty_entry)
	{
		ny = 0.0f;
		dx > 0 ? nx = -1.0f : nx = 1.0f;
	}
	else
	{
		nx = 0.0f;
		dy > 0 ? ny = -1.0f : ny = 1.0f;
	}
}

CGame* CGame::GetInstance()
{
	if (__instance == NULL) __instance = new CGame();
	return __instance;
}

#define MAX_GAME_LINE 1024

#define GAME_FILE_SECTION_UNKNOWN -1
#define GAME_FILE_SECTION_SETTINGS 1
#define GAME_FILE_SECTION_SCENES 2

void CGame::_ParseSection_SETTINGS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return;
	if (tokens[0] == "start")
		current_scene = atoi(tokens[1].c_str());
}

void CGame::_ParseSection_SCENES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return;
	int id = atoi(tokens[0].c_str());
	int type = atoi(tokens[1].c_str());
	LPCWSTR path = ToLPCWSTR(tokens[2]);

	LPSCENE scene = NULL;

	switch (type) {
	case SCENE_TYPE_INTRO:
		scene = new CIntroScene(id, path);
		break;
	case SCENE_TYPE_GAME: {
		float x = stof(tokens[3].c_str());
		float X = stof(tokens[4].c_str());
		float y = stof(tokens[5].c_str());
		float Y = stof(tokens[6].c_str());
		int xOnMap = atoi(tokens[7].c_str());
		int yOnMap = atoi(tokens[8].c_str());
		int s = atoi(tokens[9].c_str());
		scene = new CPlayScene(id, path, x, X, y, Y, xOnMap, yOnMap, s);
		break;
	}
	case SCENE_TYPE_MAP:
		scene = new CMapScene(id, path);
		break;
	}

	scenes[id] = scene;
}

void CGame::Load(LPCWSTR gameFile)
{
	ifstream f;
	f.open(gameFile);
	char str[MAX_GAME_LINE];

	int section = GAME_FILE_SECTION_UNKNOWN;

	while (f.getline(str, MAX_GAME_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;

		if (line == "[SETTINGS]") { section = GAME_FILE_SECTION_SETTINGS; continue; }
		if (line == "[SCENES]") { section = GAME_FILE_SECTION_SCENES; continue; }

		switch (section)
		{
		case GAME_FILE_SECTION_SETTINGS: _ParseSection_SETTINGS(line); break;
		case GAME_FILE_SECTION_SCENES: _ParseSection_SCENES(line); break;
		}
	}
	f.close();

	SwitchScene(current_scene);
}

void CGame::SwitchScene(int scene_id)
{
	scenes[current_scene]->Unload();

	CTextures::GetInstance()->Clear();
	CSprites::GetInstance()->Clear();
	CAnimations::GetInstance()->Clear();

	current_scene = scene_id;
	LPSCENE s = scenes[scene_id];
	CGame::GetInstance()->SetKeyHandler(s->GetKeyEventHandler());
	s->Load();

	if (dynamic_cast<CPlayScene*>(s)) {
		if (prevForm != NULL) {
			(dynamic_cast<CPlayScene*>(s))->getPlayer()->SetPCForm(prevForm);
			prevForm = NULL;
		}
		if (prevState != NULL) {
			(dynamic_cast<CPlayScene*>(s))->getPlayer()->SetState(prevState);
			prevState = NULL;
		}
		if ((newX != NULL) && (newY != NULL)) {
			(dynamic_cast<CPlayScene*>(s))->getPlayer()->SetPosition((float)newX, (float)newY);
			newX = NULL;
			newY = NULL;
		}
	}
	else {
		SetCamPos(0, 0);
		if (prevForm != NULL) {
			(dynamic_cast<CMapScene*>(s))->GetPlayer()->SetPCForm(prevForm);
			prevForm = NULL;
		}
		if (dynamic_cast<CMapScene*>(s)) {
			if ((newX != NULL) && (newY != NULL)) {
				(dynamic_cast<CMapScene*>(s))->GetPlayer()->SetPosition((float)newX, (float)newY);
				newX = NULL;
				newY = NULL;
			}
		}
	}
}