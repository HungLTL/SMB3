#include <algorithm>
#include "Tail.h"
#include "Mario.h"

#include "Goomba.h"
#include "Koopa.h"
#include "PiranhaPlant.h"

#include "GoldBlock.h"
#include "PowerBlock.h"
#include "PSwitch.h"

#include "Game.h"
#include "PlayScene.h"

CTail::CTail() {
	this->active = false;
	this->attack = 0;
	this->prevState = NULL;
	this->SetState(TAIL_STATE_DORMANT);
}

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
	CMario* mario = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->getPlayer();
	float fx, fy;
	mario->GetPosition(fx, fy);

	CGameObject::Update(dt);

	if (state != TAIL_STATE_DORMANT) {
		if (state == TAIL_STATE_ATTACK_LEFT)
			x = fx - TAIL_BBOX_WIDTH;
		else {
			if (state == TAIL_STATE_ATTACK_RIGHT)
				x = fx + MARIO_SUPER_BBOX_WIDTH;
		}
		y = fy + 18;
	}

	if ((mario->GetAttackStatus()) && (attack == 0) && (mario->GetPCForm() == MARIO_FORM_RACCOON)) {
		if (mario->GetNX() > 0) {
			SetState(TAIL_STATE_ATTACK_LEFT);
		}
		else {
			SetState(TAIL_STATE_ATTACK_RIGHT);
		}
		prevState = state;
		StartAttack();
	}

	if ((GetTickCount() - attack_start >= TAIL_WINDUP_TIME) || (GetTickCount() - attack_start >= TAIL_ATTACK_TIME))
		SetState(TAIL_STATE_DORMANT);

	if (GetTickCount() - attack_start >= TAIL_FIRSTNEUTRAL_TIME) {
		if (prevState == TAIL_STATE_ATTACK_LEFT)
			SetState(TAIL_STATE_ATTACK_RIGHT);
		else
			SetState(TAIL_STATE_ATTACK_LEFT);
	}

	if (GetTickCount() - attack_start >= TAIL_SECONDNEUTRAL_TIME)
		SetState(prevState);

	if ((GetTickCount() - attack_start >= TAIL_SECONDATTACK_TIME) || (mario->GetPCForm() != MARIO_FORM_RACCOON)) {
		SetState(TAIL_STATE_DORMANT);
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
				if (plant->GetState() != PIRANHA_STATE_DORMANT && plant->GetActive()) {
					plant->SetActive(false);
					CGame::GetInstance()->AddScore(100);
				}
			}

			if (dynamic_cast<CGoldBlock*>(coObjects->at(i))) {
				CGoldBlock* block = dynamic_cast<CGoldBlock*>(coObjects->at(i));
				if (block->GetState())
					dynamic_cast<CGoldBlock*>(coObjects->at(i))->ChangeState();
			}

			if (dynamic_cast<CPBlock*>(coObjects->at(i))) {
				CPBlock* PowerBlock = dynamic_cast<CPBlock*>(coObjects->at(i));
				if (!PowerBlock->GetState()) {
					PowerBlock->SetState(true);
					PowerBlock->ShiftY();
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

void CTail::SetState(int state) {
	CGameObject::SetState(state);
	if (state != TAIL_STATE_DORMANT)
		this->active = true;
	else
		this->active = false;
}