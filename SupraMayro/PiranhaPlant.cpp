#include "PiranhaPlant.h"

#include "Game.h"
#include "PlayScene.h"

CPiranhaPlant::CPiranhaPlant(int type) {
	this->type = type;
	this->nx = -1;
	this->IsAttacking = false;
	this->SetState(PIRANHA_STATE_DORMANT);
}

void CPiranhaPlant::SetState(int state) {
	CGameObject::SetState(state);

	switch (state) {
	case PIRANHA_STATE_DORMANT:
		vy = 0;
		StartDormant();
		break;
	case PIRANHA_STATE_EMERGE:
		vy -= PIRANHA_EMERGE_SPEED;
		break;
	case PIRANHA_STATE_ATTACK:
		vy = 0;
		IsAttacking = true;
		StartAttack();
		break;
	case PIRANHA_STATE_HIDE:
		vy += PIRANHA_EMERGE_SPEED;
		break;
	}
}

void CPiranhaPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);
	float fx, fy;
	CMario* player = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->getPlayer();
	player->GetPosition(fx, fy);
	if (fx < this->x)
		this->nx = -1;
	else
		this->nx = 1;

	if (type == PIRANHA_TYPE_FIRE) {
		if (IsAttacking) {
			float cx, cy;
			int scrnw, scrnh;
			CGame::GetInstance()->GetCamPos(cx, cy);
			scrnw = CGame::GetInstance()->GetScreenWidth();
			scrnh = CGame::GetInstance()->GetScreenHeight();

			if ((x > cx - PIRANHA_BBOX_WIDTH) && (x < cx + scrnw) && (y < cy + scrnh)) {
				CFireball* fireball = NULL;
				if (this->nx > 0)
					fireball = new CFireball(this, fx, fy, FIREBALL_PIRANHA, true);
				else
					fireball = new CFireball(this, fx, fy, FIREBALL_PIRANHA, false);
				dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->PushObject(fireball);
			}
			IsAttacking = false;
		}
	}

	if (GetTickCount() - attack_start >= PIRANHA_ATTACK_TIME) {
		if (state == PIRANHA_STATE_ATTACK) {
			attack = 0;
			attack_start = 0;
			this->SetState(PIRANHA_STATE_HIDE);
		}
	}

	if (GetTickCount() - dormant_start >= PIRANHA_DORMANT_TIME) {
		if (state == PIRANHA_STATE_DORMANT) {
			this->SetState(PIRANHA_STATE_EMERGE);
			dormant_start = 0;
			dormant = 0;
		}
	}

	if (state == PIRANHA_STATE_EMERGE) {
		if (y <= y0 - PIRANHA_BBOX_HEIGHT)
			this->SetState(PIRANHA_STATE_ATTACK);
	}

	if (state == PIRANHA_STATE_HIDE) {
		if (y >= y0)
			this->SetState(PIRANHA_STATE_DORMANT);
	}

	y += dy;
}

void CPiranhaPlant::Render() {
	int ani;
	if (type == PIRANHA_TYPE_NORMAL) {
		if (state == PIRANHA_STATE_ATTACK)
			ani = PIRANHA_ANI_BITE;
		else
			ani = PIRANHA_ANI_UP;
	}
	else {
		if (nx > 0)
			ani = PIRANHA_ANI_RIGHT;
		else
			ani = PIRANHA_ANI_LEFT;
	}
	animation_set->at(ani)->Render(x, y);
	//RenderBoundingBox();
}

void CPiranhaPlant::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = x;
	t = y;
	r = x + PIRANHA_BBOX_WIDTH;
	b = y + PIRANHA_BBOX_HEIGHT;
}