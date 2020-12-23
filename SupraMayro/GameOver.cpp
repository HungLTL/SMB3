#include "GameOver.h"

#include "Game.h"

CGameOver::CGameOver() {
	option = OPTION_CONTINUE;
	SetAnimationSet(CAnimationSets::GetInstance()->Get(GAMEOVER_ANI_SET));
}

void CGameOver::Render() {
	animation_set->at(GAMEOVER_INTERFACE)->Render(x, y);
	if (option == OPTION_CONTINUE)
		animation_set->at(GAMEOVER_CURSOR)->Render(x + 40, y + 40);
	else
		animation_set->at(GAMEOVER_CURSOR)->Render(x + 40, y + 48);
}

void CGameOver::ChangeOption() {
	if (option == OPTION_CONTINUE)
		option = OPTION_END;
	else {
		if (option == OPTION_END)
			option = OPTION_CONTINUE;
	}
}

void CGameOver::EnforceOption() {
	if (option == OPTION_CONTINUE) {
		CGame::GetInstance()->InitHUD();
		CGame::GetInstance()->SwitchScene(1);
	}
	else {
		if (option == OPTION_END)
			CGame::GetInstance()->SwitchScene(0);
	}
}