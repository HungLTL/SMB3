#include <math.h>
#include "HUD.h"

#include "Mario.h"

#include "Game.h"
#include "PlayScene.h"

CHUD::CHUD() {
	lives = vector<int>(LIVES_LENGTH);
	score = vector<int>(SCORE_LENGTH);
	coins = vector<int>(COINS_LENGTH);
	time = vector<int>(TIME_LENGTH);
	cards = vector<int>(NUM_OF_CARDS);
	charge = 0;
}

void CHUD::SplitIntoDigit(int number, vector<int> &vec, int n) {
	for (int i = n - 1, j = 0; i >= 0; i--, j++) {
		vec[i] = (int)((double)number / pow(10.0, (double)j));
		vec[i] %= 10;
	}
}

void CHUD::Update(DWORD dt) {
	int l = CGame::GetInstance()->GetLives();
	int s = CGame::GetInstance()->GetScore();
	int c = CGame::GetInstance()->GetCoins();
	int t = CGame::GetInstance()->GetTime();

	SplitIntoDigit(l, lives, LIVES_LENGTH);
	SplitIntoDigit(s, score, SCORE_LENGTH);
	SplitIntoDigit(c, coins, COINS_LENGTH);
	SplitIntoDigit(t, time, TIME_LENGTH);

	vector<int> vec = CGame::GetInstance()->GetCards();
	for (int i = 0; i < NUM_OF_CARDS; i++) {
		cards[i] = vec[i];
	}

	if (dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene()))
		charge = (dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene()))->getPlayer()->GetCharge();
}

void CHUD::RenderHUD(float StartX, float StartY, int width, int limit, vector<int> component) {
	for (int i = 0; i < limit; i++)
		animation_set->at(component[i])->Render(x + StartX + width * i, y + StartY);
}

void CHUD::Render() {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 16; j++) {
			animation_set->at(ANI_HUD_OVERLAY)->Render(x + (j * 16), y + (i * 16));
		}
	}

	animation_set->at(ANI_HUD_FRAME)->Render(x + 2, y + 2);

	RenderHUD(CARD_STARTING_X, CARD_Y, CARD_WIDTH, NUM_OF_CARDS, cards);
	RenderHUD(LIVES_STARTING_X, SECOND_ROW_Y, DIGIT_WIDTH, LIVES_LENGTH, lives);
	RenderHUD(SCORE_STARTING_X, SECOND_ROW_Y, DIGIT_WIDTH, SCORE_LENGTH, score);
	RenderHUD(COINS_STARTING_X, COINS_Y, DIGIT_WIDTH, COINS_LENGTH, coins);
	RenderHUD(TIME_STARTING_X, SECOND_ROW_Y, DIGIT_WIDTH, TIME_LENGTH, time);

	if (charge > 0) {
		for (int i = 0; i < charge && i < 6; i++)
			animation_set->at(ANI_HUD_ARROW)->Render(x + 54 + DIGIT_WIDTH * i, y + 9);
		
		if (charge == SPRINT_CHARGE_MAX)
			animation_set->at(ANI_HUD_P)->Render(x + 103, y + 9);
	}
}