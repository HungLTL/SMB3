#pragma once
#include <unordered_map>
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "Scene.h"

#define KEYBOARD_BUFFER_SIZE 1024

using namespace std;

class CGame
{
	static CGame* __instance;

	HWND hWnd;
	LPDIRECT3D9 d3d = NULL;
	LPDIRECT3DDEVICE9 d3ddv = NULL;
	LPDIRECT3DSURFACE9 backBuffer = NULL;
	LPD3DXSPRITE spriteHandler = NULL;

	LPDIRECTINPUT8       di;
	LPDIRECTINPUTDEVICE8 didv;
	BYTE  keyStates[256];
	DIDEVICEOBJECTDATA keyEvents[KEYBOARD_BUFFER_SIZE];

	LPKEYEVENTHANDLER keyHandler;

	float cam_x = 0.0f;
	float cam_y = 0.0f;

	int screen_width;
	int screen_height;

	std::unordered_map<int, LPSCENE> scenes;
	int current_scene;

	int prevState;
	int prevForm;

	int newX, newY;

	int lives, score, time, coins;
	vector<int> cards;

	void _ParseSection_SETTINGS(std::string line);
	void _ParseSection_SCENES(std::string line);

public:
	void InitKeyboard();
	void SetKeyHandler(LPKEYEVENTHANDLER handler) { keyHandler = handler; }
	void Init(HWND hWnd);
	void Draw(float x, float y, LPDIRECT3DTEXTURE9 texture, int left, int top, int right, int bottom, int alpha = 255);

	int IsKeyDown(int KeyCode);
	void ProcessKeyboard();

	void Load(LPCWSTR gameFile);
	LPSCENE GetCurrentScene() { return scenes[current_scene]; }
	void SwitchScene(int scene_id);

	int GetScreenWidth() { return screen_width; }
	int GetScreenHeight() { return screen_height; }
	void SetScreenWidth(int value) { screen_width = value; }
	void SetScreenHeight(int value) { screen_height = value; }

	int GetPrevState() { return prevState; }
	int GetPrevForm() { return prevForm; }
	void GetNewPos(int& x, int& y) { x = newX; y = newY; }

	void SetPrevState(int value) { prevState = value; }
	void SetPrevForm(int value) { prevForm = value; }
	void SetNewPos(int x, int y) { newX = x; newY = y; }

	void InitHUD();

	void AddLife() { lives++; }
	void LoseLife() { lives--; }
	int GetLives() { return lives; }

	void AddScore(int value) { score += value; }
	int GetScore() { return score; }

	void InitTimer() { time = 300; }
	void TimerTic() { if (time > 0) time--; }
	int GetTime() { return time; }

	void AddCoin();
	int GetCoins() { return coins; }

	void AddCard(int value);
	vector<int> GetCards() { return cards; }
	void ResetCards();

	static void SweptAABB(
		float ml,
		float mt,
		float mr,
		float mb,
		float dx,
		float dy,
		float sl,
		float st,
		float sr,
		float sb,
		float& t,
		float& nx,
		float& ny);

	LPDIRECT3DDEVICE9 GetDirect3DDevice() { return this->d3ddv; }
	LPDIRECT3DSURFACE9 GetBackBuffer() { return backBuffer; }
	LPD3DXSPRITE GetSpriteHandler() { return this->spriteHandler; }

	void SetCamPos(float x, float y) { cam_x = x; cam_y = y; }
	void SetCamX(float x) { cam_x = x; }
	void SetCamY(float y) { cam_y = y; }
	void GetCamPos(float& x, float& y) { x = cam_x; y = cam_y; }

	static CGame* GetInstance();

	~CGame();
};