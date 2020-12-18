#include <algorithm>
#include "Roulette.h"

#include "Mario.h"

#include "Game.h"
#include "PlayScene.h"

CRoulette::CRoulette() {
	type = ROULETTE_CARD_STAR;
	SetState(ROULETTE_STATE_IDLE);
	StartCycle();
}

void CRoulette::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents) {
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f) {
			if (dynamic_cast<CMario*>(e->obj))
				coEvents.push_back(e);
		}
		else
			delete e;
	}
	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void CRoulette::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	if ((GetTickCount() - cycle_start >= CYCLE_TIME) && (state == ROULETTE_STATE_IDLE)) {
		if (type == ROULETTE_CARD_STAR)
			type = ROULETTE_CARD_MUSHROOM;
		else
			type++;

		cycle_start = 0;
		StartCycle();
	}

	if (state == ROULETTE_STATE_ACTIVATED) {
		float fx, fy;
		CGame::GetInstance()->GetCamPos(fx, fy);
		if (y + ROULETTE_BBOX_WIDTH <= fy)
			SetState(ROULETTE_STATE_DISPLAY);
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	y += vy;

	if (coEvents.size() != 0) {
		switch (type) {
		case ROULETTE_CARD_MUSHROOM:
			CGame::GetInstance()->AddCard(ANI_HUD_MUSHROOM);
			break;
		case ROULETTE_CARD_FLOWER:
			CGame::GetInstance()->AddCard(ANI_HUD_FLOWER);
			break;
		case ROULETTE_CARD_STAR:
			CGame::GetInstance()->AddCard(ANI_HUD_STAR);
			break;
		}
		if (state == ROULETTE_STATE_IDLE)
			SetState(ROULETTE_STATE_ACTIVATED);
		dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->EndCourse();
	}
}

void CRoulette::Render() {
	int ani = -1;

	if (state == ROULETTE_STATE_DISPLAY)
		ani = ROULETTE_ANI_VICTORY_SCREEN;
	else {
		if (state == ROULETTE_STATE_IDLE) {
			switch (type) {
			case ROULETTE_CARD_MUSHROOM:
				ani = ROULETTE_ANI_MUSHROOM;
				break;
			case ROULETTE_CARD_FLOWER:
				ani = ROULETTE_ANI_FLOWER;
				break;
			case ROULETTE_CARD_STAR:
				ani = ROULETTE_ANI_STAR;
				break;
			}
		}
		else {
			if (state == ROULETTE_STATE_ACTIVATED) {
				switch (type) {
				case ROULETTE_CARD_MUSHROOM:
					ani = ROULETTE_ANI_MUSHROOM_ACTIVATED;
					break;
				case ROULETTE_CARD_FLOWER:
					ani = ROULETTE_ANI_FLOWER_ACTIVATED;
					break;
				case ROULETTE_CARD_STAR:
					ani = ROULETTE_ANI_STAR_ACTIVATED;
					break;
				}
			}
		}
	}

	animation_set->at(ani)->Render(x, y);
	if (state == ROULETTE_STATE_DISPLAY) {
		int ani2 = -1;
		switch (type) {
		case ROULETTE_CARD_MUSHROOM:
			ani2 = ROULETTE_ANI_MUSHROOM_CARD;
			break;
		case ROULETTE_CARD_FLOWER:
			ani2 = ROULETTE_ANI_FLOWER_CARD;
			break;
		case ROULETTE_CARD_STAR:
			ani2 = ROULETTE_ANI_STAR_CARD;
			break;
		}
		animation_set->at(ani2)->Render(x + 176, y + 49);
	}
}


void CRoulette::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = x;
	t = y;
	r = x + ROULETTE_BBOX_WIDTH;
	b = y + ROULETTE_BBOX_WIDTH;
}

void CRoulette::SetState(int state) {
	CGameObject::SetState(state);
	switch (state) {
	case ROULETTE_STATE_DISPLAY:
		vy = 0;
		float cx, cy;
		CGame::GetInstance()->GetCamPos(cx, cy);
		this->SetPosition(cx + 4, cy);
		break;
	case ROULETTE_STATE_IDLE:
		vy = 0;
		break;
	case ROULETTE_STATE_ACTIVATED:
		vy = -ROULETTE_ACTIVATE_SPEED;
		break;
	}
}