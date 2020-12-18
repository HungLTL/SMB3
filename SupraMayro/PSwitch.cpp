#include "PSwitch.h"
#include "GoldBlock.h"
#include "Coin.h"

#include "Mario.h"

#include "Game.h"
#include "PlayScene.h"

CPSwitch::CPSwitch() :CBlock() {
	SetState(PSWITCH_STATE_HIDDEN);
	transmute = 0;
}

void CPSwitch::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	CPlayScene* current_scene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());

	if (state == PSWITCH_STATE_ACTIVATED) {
		if (GetTickCount() - transmute_start >= TRANSMUTE_TIME) {
			for (UINT i = 1; i < (current_scene->getObjects()).size(); i++) {
				if (dynamic_cast<CCoin*>((current_scene->getObjects())[i])) {
					CCoin* coin = dynamic_cast<CCoin*>((current_scene->getObjects())[i]);

					if (coin->GetStatus()) {
						if (coin->GetOrigins()) {
							float fx, fy;
							coin->GetPosition(fx, fy);

							CGoldBlock* block = new CGoldBlock();
							block->SetPosition(fx, fy);
							block->SetAnimationSet(CAnimationSets::GetInstance()->Get(2));

							dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->Replace(i, block);
						}
					}
				}
			}
			transmute = 0;
			transmute_start = 0;
		}
	}
}

void CPSwitch::Render() {
	int ani = -1;
	switch (state) {
	case PSWITCH_STATE_HIDDEN:
		ani = PSWITCH_ANI_HIDDEN;
		animation_set->at(ani)->Render(x, y);
		break;
	case PSWITCH_STATE_IDLE:
		ani = PSWITCH_ANI_OFF;
		animation_set->at(ani)->Render(x, y);
		animation_set->at(PSWITCH_ANI_BASE)->Render(x, y + WIDTH);
		break;
	case PSWITCH_STATE_ACTIVATED:
		ani = PSWITCH_ANI_ON;
		animation_set->at(ani)->Render(x, y - WIDTH);
		animation_set->at(PSWITCH_ANI_BASE)->Render(x, y);
		break;
	}
}

void CPSwitch::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = x;
	t = y;
	r = x + WIDTH;
	if (state == PSWITCH_STATE_IDLE)
		b = y + PSWITCH_BBOX_HEIGHT;
	else
		b = y + WIDTH;
}

void CPSwitch::SetState(int state) {
	CGameObject::SetState(state);

	switch (state) {
	case PSWITCH_STATE_IDLE:
		y -= WIDTH;
		break;
	case PSWITCH_STATE_ACTIVATED: {
		y += WIDTH;

		CPlayScene* current_scene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());

		for (UINT i = 1; i < (current_scene->getObjects()).size(); i++) {
			if (dynamic_cast<CGoldBlock*>((current_scene->getObjects())[i])) {
				CGoldBlock* block = dynamic_cast<CGoldBlock*>((current_scene->getObjects())[i]);
				if (block->GetState()) {
					float fx, fy;
					block->GetPosition(fx, fy);

					CCoin* coin = new CCoin(0);
					coin->SetOrigins(true);
					coin->SetPosition(fx, fy);
					coin->SetAnimationSet(CAnimationSets::GetInstance()->Get(15));

					dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->Replace(i, coin);
				}
			}
		}

		StartTransmute();
		break;
	}
	}
}