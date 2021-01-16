#include <algorithm>
#include "Fireball.h"
#include "Coin.h"
#include "PowerUp.h"
#include "Goomba.h"
#include "Koopa.h"
#include "PiranhaPlant.h"
#include "BoomerangBro.h"
#include "Game.h"
#include "PlayScene.h"

CFireball::CFireball(CMario* player, int type) {
	float fx, fy;
	player->GetPosition(fx, fy);
	this->type = type;
	this->x1 = this->y1 = NULL;

	if (player->GetNX() > 0) {
		this->Direction = true;
		this->SetPosition(fx + 11, fy + 15);
	}
	else {
		this->Direction = false;
		this->SetPosition(fx, fy + 15);
	}

	this->SetAnimationSet((CAnimationSets::GetInstance())->Get(10));
}

CFireball::CFireball(CGameObject* host, float x, float y, int type, bool dir) {
	float fx, fy;
	host->GetPosition(fx, fy);
	this->type = type;
	this->x1 = x;
	this->y1 = y;

	if (dir) {
		this->Direction = true;
		this->SetPosition(fx + 11, fy + 10);
	}
	else {
		this->Direction = false;
		this->SetPosition(fx, fy + 10);
	}

	this->GetInitBoundaries();

	this->SetAnimationSet((CAnimationSets::GetInstance())->Get(10));
}

void CFireball::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents) {
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f) {
			if (type == FIREBALL_MARIO) {
				if (dynamic_cast<CFireball*>(e->obj) || dynamic_cast<CMario*>(e->obj) || dynamic_cast<CPowerUp*>(e->obj) || dynamic_cast<CCoin*>(e->obj)) {
					delete e;
					continue;
				}

				if (dynamic_cast<CBackgroundPlatform*>(e->obj)) {
					if (e->ny < 0)
						coEvents.push_back(e);
				}
				else {
					coEvents.push_back(e);
				}
			}
			else
				delete e;
		}
		else
			delete e;
	}
	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void CFireball::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	if (type == FIREBALL_MARIO) {
		vy += FIREBALL_GRAVITY * dt;
		if (Direction)
			vx = FIREBALL_SPEED;
		else
			vx = -FIREBALL_SPEED;
	}

	float cx, cy;
	int scrw, scrh;
	CGame::GetInstance()->GetCamPos(cx, cy);
	scrh = CGame::GetInstance()->GetScreenHeight();
	scrw = CGame::GetInstance()->GetScreenWidth();

	if (x <= cx || x > cx + scrw || y <= cy || y > cy + scrh) {
		dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(this);
		dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveFireball();
	}
	
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0) {
		if (type == FIREBALL_MARIO) {
			x += dx;
			y += dy;
		}
		else {
			float dirX, dirY;
			dirX = x1 - x0;
			dirY = y1 - y0;

			float hyp = sqrt(dirX * dirX + dirY * dirY);
			dirX /= hyp;
			dirY /= hyp;

			x += dirX * FIREBALL_PIRANHA_SPEED;
			y += dirY * FIREBALL_PIRANHA_SPEED;
		}
	}
	else {
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++) {
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGameObject*>(e->obj)) {
				if (e->ny < 0)
					vy = -FIREBALL_BOUNCE_HEIGHT;
				if (e->nx != 0) {
					dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(this);
					dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveFireball();
				}
			}

			if (dynamic_cast<CPiranhaPlant*>(e->obj)) {
				dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(e->obj);
				dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(this);
				dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveFireball();
				CGame::GetInstance()->AddScore(100);
			}

			if (dynamic_cast<CGoomba*>(e->obj)) {
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
				goomba->SetEnemyForm(GOOMBA_FORM_NORMAL);
				goomba->SetState(GOOMBA_STATE_DEATH);
				dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(this);
				dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveFireball();
			}

			if (dynamic_cast<CKoopa*>(e->obj)) {
				CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
				koopa->SetPara(false);
				koopa->SetState(KOOPA_STATE_DEATH);
				dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(this);
				dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveFireball();
			}

			if (dynamic_cast<CBoomerangBro*>(e->obj)) {
				dynamic_cast<CBoomerangBro*>(e->obj)->SetState(BBRO_STATE_DEATH);
				dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(this);
				dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveFireball();
			}
		}
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CFireball::Render() {
	int ani;
	if (Direction)
		ani = FIREBALL_RIGHT;
	else
		ani = FIREBALL_LEFT;

	animation_set->at(ani)->Render(x, y);
}

void CFireball::GetBoundingBox(float& left, float& top, float& right, float& bottom) {
	left = x;
	top = y;
	right = x + FIREBALL_BBOX_WIDTH;
	bottom = y + FIREBALL_BBOX_WIDTH;
}

CFireball::~CFireball() {
}