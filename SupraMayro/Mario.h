#pragma once
#include "GameObject.h"

#define MARIO_ACCELERATE_SPEED 0.003f
#define MARIO_SPRINT_ACCELERATE_SPEED 0.01f
#define SPRINT_CHARGE_MAX 7
#define MARIO_MIN_SPEED 0.1f
#define MAX_SPEED_PER_SPRINT_LEVEL 0.02f

#define MARIO_JUMP_SPEED_Y 0.3f
#define MARIO_INIT_FLY_SPEED 0.35f
#define MARIO_FLY_SPEED 0.02f
#define MARIO_FLY_X_DECELERATE_SPEED 0.012f
#define MARIO_RACCOON_Y_RESISTANCE 0.0015f
#define MARIO_JUMP_ACCELERATE_SPEED 0.0014f
#define MARIO_JUMP_DEFLECT_SPEED 0.25f
#define MARIO_GRAVITY 0.002f
#define MARIO_GRAVITY_FLY 0.05f
#define MARIO_DIE_DEFLECT_SPEED 0.5f

#define INVULN_TIME 1500
#define EMERGE_TIME 500
#define EMERGE_TIME_SUPER 800
#define ATTACK_TIME_RACCOON 300
#define ATTACK_TIME_FIRE 180
#define SPRINT_CHARGE_CYCLE 400
#define FLAP_TIME 320
#define FLY_DURATION 5000

#define MARIO_STATE_IDLE 0

#define MARIO_STATE_WALK_LEFT 100
#define MARIO_STATE_WALK_RIGHT 150

#define MARIO_STATE_JUMP 200

#define MARIO_STATE_SLIDE 400

#define MARIO_STATE_CROUCH 500

#define MARIO_STATE_ATTACK 600

#define MARIO_STATE_FLAP 700

#define MARIO_STATE_DEATH 800

#define MARIO_STATE_WARPING_UP 900
#define MARIO_STATE_WARPING_DOWN 950

#define MARIO_STATE_EMERGING_UP 1000
#define MARIO_STATE_EMERGING_DOWN 1050

#define MARIO_FORM_NORMAL 1
#define MARIO_FORM_SUPER 2
#define MARIO_FORM_RACCOON 3
#define MARIO_FORM_FIRE 4

#define MARIO_ANI_IDLE_NORMAL_LEFT 0
#define MARIO_ANI_IDLE_NORMAL_RIGHT 1
#define MARIO_ANI_IDLE_SUPER_LEFT 2
#define MARIO_ANI_IDLE_SUPER_RIGHT 3
#define MARIO_ANI_IDLE_RACCOON_LEFT 4
#define MARIO_ANI_IDLE_RACCOON_RIGHT 5
#define MARIO_ANI_IDLE_FIRE_LEFT 6
#define MARIO_ANI_IDLE_FIRE_RIGHT 7

#define MARIO_ANI_WALK_NORMAL_LEFT 8
#define MARIO_ANI_WALK_NORMAL_RIGHT 9
#define MARIO_ANI_WALK_SUPER_LEFT 10
#define MARIO_ANI_WALK_SUPER_RIGHT 11
#define MARIO_ANI_WALK_RACCOON_LEFT 12
#define MARIO_ANI_WALK_RACCOON_RIGHT 13
#define MARIO_ANI_WALK_FIRE_LEFT 14
#define MARIO_ANI_WALK_FIRE_RIGHT 15

#define MARIO_ANI_JUMP_NORMAL_LEFT 16
#define MARIO_ANI_JUMP_NORMAL_RIGHT 17
#define MARIO_ANI_JUMP_SUPER_LEFT 18
#define MARIO_ANI_JUMP_SUPER_RIGHT 19
#define MARIO_ANI_JUMP_RACCOON_LEFT 20
#define MARIO_ANI_JUMP_RACCOON_RIGHT 21
#define MARIO_ANI_JUMP_FIRE_LEFT 22
#define MARIO_ANI_JUMP_FIRE_RIGHT 23

#define MARIO_ANI_SPRINT_NORMAL_LEFT 24
#define MARIO_ANI_SPRINT_NORMAL_RIGHT 25
#define MARIO_ANI_SPRINT_SUPER_LEFT 26
#define MARIO_ANI_SPRINT_SUPER_RIGHT 27
#define MARIO_ANI_SPRINT_RACCOON_LEFT 28
#define MARIO_ANI_SPRINT_RACCOON_RIGHT 29
#define MARIO_ANI_SPRINT_FIRE_LEFT 30
#define MARIO_ANI_SPRINT_FIRE_RIGHT 31

#define MARIO_ANI_SLIDE_NORMAL_LEFT 32
#define MARIO_ANI_SLIDE_NORMAL_RIGHT 33
#define MARIO_ANI_SLIDE_SUPER_LEFT 34
#define MARIO_ANI_SLIDE_SUPER_RIGHT 35
#define MARIO_ANI_SLIDE_RACCOON_LEFT 36
#define MARIO_ANI_SLIDE_RACCOON_RIGHT 37
#define MARIO_ANI_SLIDE_FIRE_LEFT 38
#define MARIO_ANI_SLIDE_FIRE_RIGHT 39

#define MARIO_ANI_CROUCH_SUPER_LEFT 40
#define MARIO_ANI_CROUCH_SUPER_RIGHT 41
#define MARIO_ANI_CROUCH_RACCOON_LEFT 42
#define MARIO_ANI_CROUCH_RACCOON_RIGHT 43
#define MARIO_ANI_CROUCH_FIRE_LEFT 44
#define MARIO_ANI_CROUCH_FIRE_RIGHT 45

#define MARIO_ANI_ATTACK_RACCOON_LEFT 46
#define MARIO_ANI_ATTACK_RACCOON_RIGHT 47
#define MARIO_ANI_ATTACK_FIRE_LEFT 48
#define MARIO_ANI_ATTACK_FIRE_RIGHT 49

#define MARIO_ANI_FLY_RACCOON_LEFT 50
#define MARIO_ANI_FLY_RACCOON_RIGHT 51

#define MARIO_ANI_DEATH 52

#define MARIO_ANI_WARPING_NORMAL 53
#define MARIO_ANI_WARPING_SUPER 54
#define MARIO_ANI_WARPING_RACCOON 55
#define MARIO_ANI_WARPING_FIRE 56

#define MARIO_NORMAL_BBOX_WIDTH 12
#define MARIO_NORMAL_BBOX_HEIGHT 15
#define MARIO_SUPER_BBOX_WIDTH 15
#define MARIO_SUPER_BBOX_HEIGHT 27
#define MARIO_CROUCH_BBOX_WIDTH 14
#define MARIO_CROUCH_BBOX_HEIGHT 18

class CMario :public CGameObject {
	int form;
	float start_x, start_y;

	int SprintChargeLevel;

	bool IsGrounded, IsAttacking, IsCarrying, IsChargingJump, IsHoldingDownW, IsChargingSprint, IsFlying, IsFlapping;
	int AttackTime;
	int invuln;
	DWORD invuln_start;
	DWORD emerge_start;
	DWORD attack_start;
	int sprint;
	DWORD sprint_start;
	DWORD fly_start;
	DWORD flap_start;
public:
	CMario(float x = 0.0f, float y = 0.0f);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();

	void SetState(int state);
	void SetPCForm(int form);
	int GetPCForm() { return this->form; }
	int GetNX() { return this->nx; }
	float GetVY() { return this->vy; }
	int GetAttackTime(){ return this->AttackTime; }
	int GetCharge() { return this->SprintChargeLevel; }

	bool GetGravStatus() { return this->IsGrounded; }
	bool GetAttackStatus() { return this->IsAttacking; }
	bool GetCarryStatus() { return this->IsCarrying; }
	bool GetFlyStatus() { return this->IsFlying; }
	void SetCarryStatus(bool result) { this->IsCarrying = result; }
	void SetWButtonStatus(bool result) { this->IsHoldingDownW = result; }
	void SetJumpCharge(bool value) { this->IsChargingJump = value; }
	void SetSprintCharge(bool value) { this->IsChargingSprint = value; }
	void ToggleSprint() { if (sprint == 0) sprint = 1; }

	void StartInvuln() { invuln = 1; invuln_start = GetTickCount(); }
	void StartEmerge() { emerge_start = GetTickCount(); }
	void StartAttack() { attack_start = GetTickCount(); }
	void StartSprint() { sprint_start = GetTickCount(); }
	void StartFly() { fly_start = GetTickCount(); }
	void StartFlapping() { flap_start = GetTickCount(); }

	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};