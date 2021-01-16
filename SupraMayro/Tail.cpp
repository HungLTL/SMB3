#include <algorithm>
#include "Tail.h"
#include "Mario.h"

#include "Goomba.h"
#include "Koopa.h"
#include "PiranhaPlant.h"
#include "BoomerangBro.h"

#include "GoldBlock.h"
#include "CoinBlock.h"
#include "PowerBlock.h"
#include "PSwitch.h"

#include "Game.h"
#include "PlayScene.h"

bool CTail::CheckOverlap(LPGAMEOBJECT obj) {
	float l, t, r, b;
	float lo, to, ro, bo;
	this->GetBoundingBox(l, t, r, b);
	obj->GetBoundingBox(lo, to, ro, bo);

	if (l < ro && r > lo && t < bo && b > to)
		return true;
	else
		return false;
}

void CTail::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	CMario* mario = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->getPlayer();
	float fx, fy;
	mario->GetPosition(fx, fy);

	if (mario->GetNX() < 0)
		x = fx - TAIL_BBOX_WIDTH;
	else
		x = fx + MARIO_SUPER_BBOX_WIDTH;

	y = fy + 18;

	if ((mario->GetAttackStatus()) && (attack == 0) && (mario->GetPCForm() == MARIO_FORM_RACCOON))
		StartAttack();

	if (GetTickCount() - attack_start > TAIL_DAMAGE_TIME)
		active = true;

	if (GetTickCount() - attack_start > TAIL_DOWN_TIME)
		active = false;

	if ((GetTickCount() - attack_start >= TAIL_ATTACK_SEQ) || (mario->GetPCForm() != MARIO_FORM_RACCOON)) {
		attack = 0;
		attack_start = 0;
	}
	
	
	for (UINT i = 0; i < coObjects->size(); i++) {
		if (CheckOverlap(coObjects->at(i))) {
			if (dynamic_cast<CGoomba*>(coObjects->at(i))) {
				CGoomba* goomba = dynamic_cast<CGoomba*>(coObjects->at(i));
				if (goomba->GetState() != GOOMBA_STATE_DEATH) {
					goomba->SetEnemyForm(GOOMBA_FORM_NORMAL);
					goomba->SetState(GOOMBA_STATE_DEATH);
				}
			}

			if (dynamic_cast<CKoopa*>(coObjects->at(i))) {
				CKoopa* koopa = dynamic_cast<CKoopa*>(coObjects->at(i));
				if (koopa->GetState() != KOOPA_STATE_DEATH) {
					koopa->SetPara(false);
					koopa->SetState(KOOPA_STATE_DEATH);
				}
			}

			if (dynamic_cast<CPiranhaPlant*>(coObjects->at(i))) {
				CPiranhaPlant* plant = dynamic_cast<CPiranhaPlant*>(coObjects->at(i));
				if (plant->GetState() != PIRANHA_STATE_DORMANT) {
					dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(plant);
					CGame::GetInstance()->AddScore(100);
				}
			}

			if (dynamic_cast<CBoomerangBro*>(coObjects->at(i)))
				dynamic_cast<CBoomerangBro*>(coObjects->at(i))->SetState(BBRO_STATE_DEATH);

			if (dynamic_cast<CGoldBlock*>(coObjects->at(i))) {
				if (dynamic_cast<CPBlock*>(coObjects->at(i))) {
					CPBlock* PowerBlock = dynamic_cast<CPBlock*>(coObjects->at(i));
					if (!PowerBlock->GetState()) {
						float pbx, pby; PowerBlock->GetPosition(pbx, pby = 0);

						if (x < pbx)
							PowerBlock->ChangeActivationSide();
						PowerBlock->SetState(true);
						PowerBlock->SetStatus();
					}
				}
				else {
					if (dynamic_cast<CCoinBlock*>(coObjects->at(i)))
						dynamic_cast<CCoinBlock*>(coObjects->at(i))->SetStatus();
					else {
						dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(coObjects->at(i));
						CGame::GetInstance()->AddScore(10);
					}
				}
			}

			if (dynamic_cast<CPSwitch*>(coObjects->at(i))) {
				CPSwitch* pswitch = dynamic_cast<CPSwitch*>(coObjects->at(i));
				if (pswitch->GetState() == PSWITCH_STATE_HIDDEN)
					pswitch->SetState(PSWITCH_STATE_IDLE);
			}
		}
	}
}

void CTail::GetBoundingBox(float& left, float& top, float& right, float& bottom) {
	if (active) {
		left = x;
		top = y;
		right = x + TAIL_BBOX_WIDTH;
		bottom = y + TAIL_BBOX_HEIGHT;
	}
}