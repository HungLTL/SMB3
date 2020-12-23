#pragma once
#include "BackgroundTile.h"

#define OPTION_CONTINUE 0
#define OPTION_END 1

#define GAMEOVER_INTERFACE 0
#define GAMEOVER_CURSOR 1

#define GAMEOVER_ANI_SET 50

class CGameOver :public CBackgroundTile {
	int option;
public:
	CGameOver();

	void Render();

	void ChangeOption();
	void EnforceOption();
};