#pragma once
#include "BackgroundObject.h"

#define LIVES_LENGTH 2
#define SCORE_LENGTH 7
#define COINS_LENGTH 2
#define TIME_LENGTH 3
#define NUM_OF_CARDS 3

#define ANI_HUD_0 0
#define ANI_HUD_1 1
#define ANI_HUD_2 2
#define ANI_HUD_3 3
#define ANI_HUD_4 4
#define ANI_HUD_5 5
#define ANI_HUD_6 6
#define ANI_HUD_7 7
#define ANI_HUD_8 8
#define ANI_HUD_9 9

#define ANI_HUD_ARROW 10
#define ANI_HUD_P 11

#define ANI_HUD_EMPTY 12
#define ANI_HUD_MUSHROOM 13
#define ANI_HUD_FLOWER 14
#define ANI_HUD_STAR 15

#define ANI_HUD_FRAME 16
#define ANI_HUD_OVERLAY 17

#define COINS_STARTING_X 134
#define COINS_Y 9
#define LIVES_STARTING_X 31
#define SCORE_STARTING_X 54
#define TIME_STARTING_X 126
#define SECOND_ROW_Y 17

#define CARD_STARTING_X 167
#define CARD_Y 2

#define DIGIT_WIDTH 8
#define CARD_WIDTH 24

class CHUD :public CBackgroundObject {
	vector<int> lives, score, coins, time, cards;
	int charge;
public:
	CHUD();

	void SplitIntoDigit(int number, vector<int> &vec, int n);
	void RenderHUD(float StartX, float StartY, int width, int limit, vector<int> component);

	void Update(DWORD dt);
	void Render();
};