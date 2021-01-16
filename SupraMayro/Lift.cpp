#include "Lift.h"

#include "Game.h"
#include "PlayScene.h"

void CLift::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	if (state == LIFT_STATE_IDLE) {
		float fx, fy = 0.0f;
		CGame::GetInstance()->GetCamPos(fx, fy);
		float scrw = (float)CGame::GetInstance()->GetScreenWidth();

		if (fx >= x - WIDTH - scrw)
			SetState(LIFT_STATE_MOVING);
	}

	if (state == LIFT_STATE_FALLING)
		vy += LIFT_GRAVITY * dt;

	x += dx;
	y += dy;
}

void CLift::Render() {
	animation_set->at(LIFT_ANI_LEFT)->Render(x, y);
	animation_set->at(LIFT_ANI_MID)->Render(x + WIDTH, y);
	animation_set->at(LIFT_ANI_RIGHT)->Render(x + WIDTH * 2, y);
}

void CLift::SetState(int state) {
	CGameObject::SetState(state);

	switch (state) {
	case LIFT_STATE_IDLE:
		vx = vy = 0;
		break;
	case LIFT_STATE_MOVING:
		vx = -LIFT_SPEED;
		break;
	case LIFT_STATE_FALLING:
		vx = 0;
		break;
	}
}

void CLift::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = x;
	t = y;
	r = x + WIDTH * 3;
	b = y + WIDTH;
}