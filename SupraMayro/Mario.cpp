#include <algorithm>
#include <assert.h>
#include "Mario.h"
#include "Utility.h"
#include "PlayScene.h"
#include "Game.h"
#include "Lift.h"
#include "GoldBlock.h"
#include "CoinBlock.h"
#include "PSwitch.h"
#include "PowerBlock.h"
#include "Fireball.h"
#include "Boomerang.h"
#include "Goomba.h"
#include "Koopa.h"
#include "PiranhaPlant.h"
#include "BoomerangBro.h"
#include "Roulette.h"
#include "Portal.h"

#include "Game.h"

CMario::CMario(float x, float y) :CGameObject() {
	SetPCForm(MARIO_FORM_NORMAL);
	SetState(MARIO_STATE_IDLE);

	SprintChargeLevel = 0;

	IsGrounded = true;
	IsCarrying = IsChargingJump = IsChargingSprint = IsHoldingDownW = IsAttacking = IsFlying = IsFlapping = IsUsingLift = false;
	sprint = invuln = 0;
	emerge_start = attack_start = invuln_start = sprint_start = 0;

	grav_limit_x = grav_limit_X = 0;

	start_x = x;
	start_y = y;

	this->x = x;
	this->y = y;
}

void CMario::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents) {
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f) {
			if ((state == MARIO_STATE_WARPING_UP) || (state == MARIO_STATE_WARPING_DOWN)) {
				if (dynamic_cast<CPortal*>(e->obj))
					coEvents.push_back(e);
				else
					delete e;
				continue;
			}

			if ((dynamic_cast<CCoin*>(e->obj)) || (dynamic_cast<CPowerUp*>(e->obj)) || (dynamic_cast<CRoulette*>(e->obj))) {
				delete e;
				continue;
			}

			if (dynamic_cast<CFireball*>(e->obj)) {
				CFireball* fireball = dynamic_cast<CFireball*>(e->obj);
				if (fireball->GetType() == FIREBALL_PIRANHA)
					coEvents.push_back(e);
				else
					delete e;
				continue;
			}

			if ((dynamic_cast<CBackgroundPlatform*>(e->obj))) {
				if (dynamic_cast<CBackgroundPlatform*>(e->obj)) {
					if (e->ny < 0)
						coEvents.push_back(e);
				}
			}
			else {
				coEvents.push_back(e);
			}
		}
		else
			delete e;
	}
	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);

	float MaxSpeed = MARIO_MIN_SPEED + MAX_SPEED_PER_SPRINT_LEVEL * SprintChargeLevel;
	if (sprint == 1) {
		if (GetTickCount() - sprint_start >= SPRINT_CHARGE_CYCLE) {
			if ((!IsChargingSprint) || (nx > 0 && vx < 0) || (nx < 0 && vx > 0) || (vx == 0) || (IsFlying)) {
				if (SprintChargeLevel > 0)
					SprintChargeLevel--;
				else {
					SprintChargeLevel = sprint = 0;
					sprint_start = 0;
				}
			}
			else {
				if ((IsChargingSprint) && (vx != 0)) {
					if (SprintChargeLevel < SPRINT_CHARGE_MAX)
						SprintChargeLevel++;
				}
			}
			sprint_start = 0;
			StartSprint();
		}
	}

	if (IsFlying) {
		MaxSpeed = MARIO_MIN_SPEED;
		SprintChargeLevel = SPRINT_CHARGE_MAX;
	}

	if (GetTickCount() - fly_start >= FLY_DURATION) {
		IsFlying = false;
		fly_start = 0;
	}

	if (IsFlapping) {
		if (GetTickCount() - flap_start >= FLAP_TIME) {
			flap_start = 0;
			IsFlapping = false;
		}
	}

	if (nx > 0) {
		if (vx > MaxSpeed) {
			if (!IsFlying)
				vx = MaxSpeed;
			else
				vx -= MARIO_FLY_X_DECELERATE_SPEED;
		}
	}
	else {
		if (nx < 0) {
			if (vx < -MaxSpeed) {
				if (!IsFlying)
					vx = -MaxSpeed;
				else
					vx += MARIO_FLY_X_DECELERATE_SPEED;
			}
		}
	}

	if (IsUsingLift) {
		vy += LIFT_GRAVITY * dt;
		if (!IsGrounded)
			IsGrounded = true;

		int mw = 0;
		if (form == MARIO_FORM_NORMAL)
			mw = MARIO_NORMAL_BBOX_WIDTH;
		else
			mw = MARIO_SUPER_BBOX_WIDTH;

		if ((x + mw < grav_limit_x) || (x >= grav_limit_X))
			IsUsingLift = false;
	}
	else {
		if (vy != 0)
			IsGrounded = false;
		else
			IsGrounded = true;
	}

	if (IsAttacking) {
		DWORD attack_time = 0;
		if (form == MARIO_FORM_RACCOON)
			attack_time = ATTACK_TIME_RACCOON;
		else {
			if (form == MARIO_FORM_FIRE)
				attack_time = ATTACK_TIME_FIRE;
		}

		if ((GetTickCount() - attack_start == 0) && (form == MARIO_FORM_FIRE)) {
			dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->AddFireball();
			CFireball* fireball = new CFireball(this, FIREBALL_MARIO);
			dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->PushObject(fireball);
		}

		if (GetTickCount() - attack_start >= attack_time) {
			IsAttacking = false;
			attack_start = 0;
		}
	}

	if (vy < 0) {
		if (IsChargingJump)
			vy -= MARIO_JUMP_ACCELERATE_SPEED * dt;
	}

	if ((state != MARIO_STATE_WARPING_UP)
		&& (state != MARIO_STATE_EMERGING_UP)
		&& (state != MARIO_STATE_EMERGING_DOWN)
		&& (state != MARIO_STATE_WARPING_DOWN)
		&& (!IsFlapping)
		&& (!IsUsingLift))
		vy += MARIO_GRAVITY * dt;

	if ((state == MARIO_STATE_EMERGING_UP) || (state == MARIO_STATE_EMERGING_DOWN)) {
		DWORD emerge_time = 0;
		if (form == MARIO_FORM_NORMAL)
			emerge_time = EMERGE_TIME;
		else
			emerge_time = EMERGE_TIME_SUPER;
		if (GetTickCount() - emerge_start >= emerge_time) {
			emerge_start = 0;
			SetState(MARIO_STATE_IDLE);
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != MARIO_STATE_DEATH)
		CalcPotentialCollisions(coObjects, coEvents);

	if (GetTickCount() - invuln_start > INVULN_TIME)
	{
		invuln_start = 0;
		invuln = 0;
	}

	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		x += min_tx * dx + nx * 0.4f;
		y += min_ty * dy + ny * 0.4f;

		if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;


		for (UINT i = 0; i < coEventsResult.size(); i++) {
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGameObject*>(e->obj)) {
				if (e->ny < 0) {
					IsGrounded = true;
				}
			}

			if (dynamic_cast<CLift*>(e->obj)) {
				if (e->ny < 0) {
					CLift* lift = dynamic_cast<CLift*>(e->obj);
					if (lift->GetState() == LIFT_STATE_MOVING) {
						lift->SetState(LIFT_STATE_FALLING);
						IsUsingLift = true;
						lift->GetLiftCoordinates(grav_limit_x, grav_limit_X);
					}
				}
				if (e->nx < 0)
					x += min_tx * dx + nx;
			}

			if (dynamic_cast<CPipe*>(e->obj)) {
				CPipe* pipe = dynamic_cast<CPipe*>(e->obj);
				if (pipe->GetWarp()) {
					int pipePos = (int)pipe->GetX();
					int warpStart = pipePos + 2;
					int warpEnd = pipePos + (WIDTH * 2) - 2 - MARIO_SUPER_BBOX_WIDTH;

					if ((x >= warpStart) && (x <= warpEnd)) {
						if ((state == MARIO_STATE_CROUCH) && (e->ny < 0))
							SetState(MARIO_STATE_WARPING_DOWN);
						else {
							if (e->ny > 0) {
								if (IsHoldingDownW) {
									SetState(MARIO_STATE_WARPING_UP);
									IsHoldingDownW = false;
								}
							}
						}
					}
				}
			}

			if (dynamic_cast<CGoldBlock*>(e->obj)) {
				if (dynamic_cast<CPBlock*>(e->obj)) {
					CPBlock* PowerBlock = dynamic_cast<CPBlock*>(e->obj);
					if (e->ny > 0) {
						if (!PowerBlock->GetState()) {
							float pbx, pby; PowerBlock->GetPosition(pbx, pby);
							float pblock_mid = pbx + WIDTH / 2;

							int mw = 0;
							if (form == MARIO_FORM_NORMAL)
								mw = MARIO_NORMAL_BBOX_WIDTH;
							else
								mw = MARIO_SUPER_BBOX_WIDTH;
							float m_mid = x + (float)(mw / 2);

							if (m_mid < pblock_mid)
								PowerBlock->ChangeActivationSide();
							PowerBlock->SetState(true);
							PowerBlock->SetStatus();
						}
					}
				}
				else {
					if (dynamic_cast<CCoinBlock*>(e->obj)) {
						if (e->ny > 0)
							dynamic_cast<CCoinBlock*>(e->obj)->SetStatus();
					}
					else {
						if (e->ny > 0) {
							if (form == MARIO_FORM_NORMAL)
								dynamic_cast<CGoldBlock*>(e->obj)->SetStatus();
							else {
								dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->RemoveObject(e->obj);
								CGame::GetInstance()->AddScore(10);
							}
						}
					}
				}
			}

			if (dynamic_cast<CPSwitch*>(e->obj)) {
				CPSwitch* pswitch = dynamic_cast<CPSwitch*>(e->obj);
				switch (pswitch->GetState()) {
				case PSWITCH_STATE_HIDDEN:
					if (((e->ny > 0) && (form != MARIO_FORM_NORMAL)))
						pswitch->SetState(PSWITCH_STATE_IDLE);
					break;
				case PSWITCH_STATE_IDLE:
					if (e->ny < 0)
						pswitch->SetState(PSWITCH_STATE_ACTIVATED);
					break;
				}
			}

			if (dynamic_cast<CGoomba*>(e->obj))
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

				if (e->ny < 0)
				{
					if (goomba->GetEnemyForm() == GOOMBA_FORM_NORMAL) {
						if (goomba->GetState() != GOOMBA_STATE_FLATTENED)
						{
							goomba->SetState(GOOMBA_STATE_FLATTENED);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
						}
					}
					else {
						goomba->SetEnemyForm(GOOMBA_FORM_NORMAL);
						if (goomba->GetPrevState() == GOOMBA_STATE_WALK_LEFT)
							goomba->SetState(GOOMBA_STATE_WALK_LEFT);
						else
							goomba->SetState(GOOMBA_STATE_WALK_RIGHT);
						vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
				}
				else
				{
					if (invuln == 0)
					{
						if (goomba->GetState() != GOOMBA_STATE_DEATH)
						{
							switch (form) {
							case MARIO_FORM_RACCOON:
							case MARIO_FORM_FIRE:
								SetPCForm(MARIO_FORM_SUPER);
								StartInvuln();
								break;
							case MARIO_FORM_SUPER:
								SetPCForm(MARIO_FORM_NORMAL);
								StartInvuln();
								break;
							case MARIO_FORM_NORMAL:
								SetState(MARIO_STATE_DEATH);
								break;
							}
						}
					}
					else
						y--;
				}
			}

			if (dynamic_cast<CKoopa*>(e->obj)) {
				CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);

				if (koopa->GetState() == KOOPA_STATE_DORMANT) {
					if (!IsCarrying) {
						if ((e->nx != 0) || (e->ny > 0)) {
							StartInvuln();
							if (this->nx > 0)
								koopa->SetState(KOOPA_STATE_PINBALL_RIGHT);
							else
								koopa->SetState(KOOPA_STATE_PINBALL_LEFT);
						}
					}
					else {
						koopa->SetCarryStatus(true);
					}
				}

				if (e->ny < 0) {
					switch (koopa->GetState()) {
					case KOOPA_STATE_WALK_LEFT:
					case KOOPA_STATE_WALK_RIGHT:
						if (koopa->GetPara())
							koopa->SetPara(false);
						else {
							koopa->SetState(KOOPA_STATE_DORMANT);
							koopa->ModY(KOOPA_BBOX_HEIGHT - KOOPA_SHELL_BBOX_HEIGHT);
						}
						break;
					case KOOPA_STATE_PINBALL_LEFT:
					case KOOPA_STATE_PINBALL_RIGHT:
						koopa->SetState(KOOPA_STATE_DORMANT);
						break;
					case KOOPA_STATE_DORMANT:
						if (this->nx > 0)
							koopa->SetState(KOOPA_STATE_PINBALL_RIGHT);
						else
							koopa->SetState(KOOPA_STATE_PINBALL_LEFT);
						break;
					}
					vy = -MARIO_JUMP_DEFLECT_SPEED;
				}
				else
				{
					if (invuln == 0)
					{
						if ((koopa->GetState() != KOOPA_STATE_DEATH) && (koopa->GetState() != KOOPA_STATE_DORMANT))
						{
							switch (form) {
							case MARIO_FORM_RACCOON:
							case MARIO_FORM_FIRE:
								SetPCForm(MARIO_FORM_SUPER);
								StartInvuln();
								break;
							case MARIO_FORM_SUPER:
								SetPCForm(MARIO_FORM_NORMAL);
								StartInvuln();
								break;
							case MARIO_FORM_NORMAL:
								SetState(MARIO_STATE_DEATH);
								break;
							}
						}
					}
					else
						y--;
				}
			}

			if (dynamic_cast<CBoomerangBro*>(e->obj)) {
				if (e->ny < 0) {
					dynamic_cast<CBoomerangBro*>(e->obj)->SetState(BBRO_STATE_DEATH);
					vy = -MARIO_JUMP_DEFLECT_SPEED;
				}
				else {
					if (invuln == 0)
					{
						switch (form) {
						case MARIO_FORM_RACCOON:
						case MARIO_FORM_FIRE:
							SetPCForm(MARIO_FORM_SUPER);
							StartInvuln();
							break;
						case MARIO_FORM_SUPER:
							SetPCForm(MARIO_FORM_NORMAL);
							StartInvuln();
							break;
						case MARIO_FORM_NORMAL:
							SetState(MARIO_STATE_DEATH);
							break;
						}
					}
				}
			}

			if ((dynamic_cast<CPiranhaPlant*>(e->obj)) || (dynamic_cast<CFireball*>(e->obj)) || (dynamic_cast<CBoomerang*>(e->obj))) {
				if (invuln == 0)
				{
					switch (form) {
					case MARIO_FORM_RACCOON:
					case MARIO_FORM_FIRE:
						SetPCForm(MARIO_FORM_SUPER);
						StartInvuln();
						break;
					case MARIO_FORM_SUPER:
						SetPCForm(MARIO_FORM_NORMAL);
						StartInvuln();
						break;
					case MARIO_FORM_NORMAL:
						SetState(MARIO_STATE_DEATH);
						break;
					}
				}
			}

			if (dynamic_cast<CPortal*>(e->obj)) {
				CPortal* portal = dynamic_cast<CPortal*>(e->obj);

				if (state == MARIO_STATE_WARPING_DOWN) {
					if (portal->GetDirection())
						CGame::GetInstance()->SetPrevState(MARIO_STATE_EMERGING_DOWN);
					else
						CGame::GetInstance()->SetPrevState(MARIO_STATE_EMERGING_UP);
				}
				else {
					if (state == MARIO_STATE_WARPING_UP) {
						if (portal->GetDirection())
							CGame::GetInstance()->SetPrevState(MARIO_STATE_EMERGING_UP);
						else
							CGame::GetInstance()->SetPrevState(MARIO_STATE_EMERGING_DOWN);
					}
				}

				CGame::GetInstance()->SetPrevForm(form);
				CGame::GetInstance()->SetNewPos(portal->GetNewX(), portal->GetNewY());
				CGame::GetInstance()->SwitchScene(portal->GetSceneID());
			}
		}
	}

	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CMario::Render() {
	int ani = -1;
	if (state == MARIO_STATE_DEATH)
		ani = MARIO_ANI_DEATH;
	else {
		switch (form) {
		case MARIO_FORM_NORMAL: {
			if ((state == MARIO_STATE_WARPING_UP) || (state == MARIO_STATE_EMERGING_UP) || (state == MARIO_STATE_EMERGING_DOWN) || (state == MARIO_STATE_WARPING_DOWN))
				ani = MARIO_ANI_WARPING_NORMAL;
			else {
				if (!IsGrounded) {
					if (SprintChargeLevel == SPRINT_CHARGE_MAX) {
						if (nx > 0)
							ani = MARIO_ANI_FLY_NORMAL_RIGHT;
						else
							ani = MARIO_ANI_FLY_NORMAL_LEFT;
					}
					else {
						if (nx > 0)
							ani = MARIO_ANI_JUMP_NORMAL_RIGHT;
						else
							ani = MARIO_ANI_JUMP_NORMAL_LEFT;
					}
				}
				else {
					if (vx == 0) {
						if (nx > 0) {
							ani = MARIO_ANI_IDLE_NORMAL_RIGHT;
						}
						else
							ani = MARIO_ANI_IDLE_NORMAL_LEFT;
					}
					else {
						if (vx > 0) {
							if (nx < 0)
								ani = MARIO_ANI_SLIDE_NORMAL_LEFT;
							else {
								if (SprintChargeLevel == SPRINT_CHARGE_MAX)
									ani = MARIO_ANI_SPRINT_NORMAL_RIGHT;
								else
									ani = MARIO_ANI_WALK_NORMAL_RIGHT;
							}
						}
						else {
							if (vx < 0) {
								if (nx > 0)
									ani = MARIO_ANI_SLIDE_NORMAL_RIGHT;
								else {
									if (SprintChargeLevel == SPRINT_CHARGE_MAX)
										ani = MARIO_ANI_SPRINT_NORMAL_LEFT;
									else
										ani = MARIO_ANI_WALK_NORMAL_LEFT;
								}
							}
						}
					}
				}
			}
			break;
		}
		case MARIO_FORM_SUPER: {
			if ((state == MARIO_STATE_WARPING_UP) || (state == MARIO_STATE_EMERGING_UP) || (state == MARIO_STATE_EMERGING_DOWN) || (state == MARIO_STATE_WARPING_DOWN))
				ani = MARIO_ANI_WARPING_SUPER;
			else {
				if (!IsGrounded) {
					if (SprintChargeLevel == SPRINT_CHARGE_MAX) {
						if (nx > 0)
							ani = MARIO_ANI_FLY_SUPER_RIGHT;
						else
							ani = MARIO_ANI_FLY_SUPER_LEFT;
					}
					else {
						if (nx > 0)
							ani = MARIO_ANI_JUMP_SUPER_RIGHT;
						else
							ani = MARIO_ANI_JUMP_SUPER_LEFT;
					}
				}
				else {
					if (vx == 0) {
						if (state == MARIO_STATE_CROUCH) {
							if (nx > 0)
								ani = MARIO_ANI_CROUCH_SUPER_RIGHT;
							else
								ani = MARIO_ANI_CROUCH_SUPER_LEFT;
						}
						else {
							if (nx > 0)
								ani = MARIO_ANI_IDLE_SUPER_RIGHT;
							else
								ani = MARIO_ANI_IDLE_SUPER_LEFT;
						}
					}
					else {
						if (vx > 0) {
							if (nx < 0)
								ani = MARIO_ANI_SLIDE_SUPER_LEFT;
							else {
								if (SprintChargeLevel == SPRINT_CHARGE_MAX)
									ani = MARIO_ANI_SPRINT_SUPER_RIGHT;
								else
									ani = MARIO_ANI_WALK_SUPER_RIGHT;
							}
						}
						else {
							if (vx < 0) {
								if (nx > 0)
									ani = MARIO_ANI_SLIDE_SUPER_RIGHT;
								else {
									if (SprintChargeLevel == SPRINT_CHARGE_MAX)
										ani = MARIO_ANI_SPRINT_SUPER_LEFT;
									else
										ani = MARIO_ANI_WALK_SUPER_LEFT;
								}
							}
						}
					}
				}
			}
			break;
		}
		case MARIO_FORM_RACCOON: {
			if ((state == MARIO_STATE_WARPING_UP) || (state == MARIO_STATE_EMERGING_UP) || (state == MARIO_STATE_EMERGING_DOWN) || (state == MARIO_STATE_WARPING_DOWN))
				ani = MARIO_ANI_WARPING_RACCOON;
			else {
				if (IsAttacking) {
					if (nx > 0)
						ani = MARIO_ANI_ATTACK_RACCOON_RIGHT;
					else
						ani = MARIO_ANI_ATTACK_RACCOON_LEFT;
				}
				else {
					if (!IsGrounded) {
						if (IsFlapping) {
							if (SprintChargeLevel == SPRINT_CHARGE_MAX) {
								if (nx > 0)
									ani = MARIO_ANI_FLY_RACCOON_RIGHT;
								else
									ani = MARIO_ANI_FLY_RACCOON_LEFT;
							}
							else {
								if (nx > 0)
									ani = MARIO_ANI_FLAPPING_RIGHT;
								else
									ani = MARIO_ANI_FLAPPING_LEFT;
							}
						}
						else {
							if (SprintChargeLevel == SPRINT_CHARGE_MAX) {
								if (nx > 0)
									ani = MARIO_ANI_HIJUMP_RIGHT;
								else
									ani = MARIO_ANI_HIJUMP_LEFT;
							}
							else {
								if (nx > 0)
									ani = MARIO_ANI_JUMP_RACCOON_RIGHT;
								else
									ani = MARIO_ANI_JUMP_RACCOON_LEFT;
							}
						}
					}
					else {
						if (vx == 0) {
							if (state == MARIO_STATE_CROUCH) {
								if (nx > 0)
									ani = MARIO_ANI_CROUCH_RACCOON_RIGHT;
								else
									ani = MARIO_ANI_CROUCH_RACCOON_LEFT;
							}
							else {
								if (nx > 0)
									ani = MARIO_ANI_IDLE_RACCOON_RIGHT;
								else
									ani = MARIO_ANI_IDLE_RACCOON_LEFT;
							}
						}
						else {
							if (vx > 0) {
								if (nx < 0)
									ani = MARIO_ANI_SLIDE_RACCOON_LEFT;
								else {
									if (SprintChargeLevel == SPRINT_CHARGE_MAX)
										ani = MARIO_ANI_SPRINT_RACCOON_RIGHT;
									else
										ani = MARIO_ANI_WALK_RACCOON_RIGHT;
								}
							}
							else {
								if (vx < 0) {
									if (nx > 0)
										ani = MARIO_ANI_SLIDE_RACCOON_RIGHT;
									else {
										if (SprintChargeLevel == SPRINT_CHARGE_MAX)
											ani = MARIO_ANI_SPRINT_RACCOON_LEFT;
										else
											ani = MARIO_ANI_WALK_RACCOON_LEFT;
									}
								}
							}
						}
					}
				}
			}
			break;
		}
		case MARIO_FORM_FIRE: {
			if ((state == MARIO_STATE_WARPING_UP) || (state == MARIO_STATE_EMERGING_UP) || (state == MARIO_STATE_EMERGING_DOWN) || (state == MARIO_STATE_WARPING_DOWN))
				ani = MARIO_ANI_WARPING_FIRE;
			else {
				if (!IsGrounded) {
					if (SprintChargeLevel == SPRINT_CHARGE_MAX) {
						if (nx > 0)
							ani = MARIO_ANI_FLY_FIRE_RIGHT;
						else
							ani = MARIO_ANI_FLY_FIRE_LEFT;
					}
					else {
						if (nx > 0)
							ani = MARIO_ANI_JUMP_FIRE_RIGHT;
						else
							ani = MARIO_ANI_JUMP_FIRE_LEFT;
					}
				}
				else {
					if (state == MARIO_STATE_ATTACK) {
						if (nx > 0)
							ani = MARIO_ANI_WALK_FIRE_RIGHT;
						else
							ani = MARIO_ANI_WALK_FIRE_LEFT;
					}
					else {
						if (vx == 0) {
							if (state == MARIO_STATE_CROUCH) {
								if (nx > 0)
									ani = MARIO_ANI_CROUCH_FIRE_RIGHT;
								else
									ani = MARIO_ANI_CROUCH_FIRE_LEFT;
							}
							else {
								if (IsAttacking) {
									if (nx > 0)
										ani = MARIO_ANI_ATTACK_FIRE_RIGHT;
									else
										ani = MARIO_ANI_ATTACK_FIRE_LEFT;
								}
								else {
									if (nx > 0)
										ani = MARIO_ANI_IDLE_FIRE_RIGHT;
									else
										ani = MARIO_ANI_IDLE_FIRE_LEFT;
								}
							}
						}
						else {
							if (vx > 0) {
								if (nx < 0)
									ani = MARIO_ANI_SLIDE_FIRE_LEFT;
								else {
									if (SprintChargeLevel == SPRINT_CHARGE_MAX)
										ani = MARIO_ANI_SPRINT_FIRE_RIGHT;
									else
										ani = MARIO_ANI_WALK_FIRE_RIGHT;
								}
							}
							else {
								if (vx < 0) {
									if (nx > 0)
										ani = MARIO_ANI_SLIDE_FIRE_RIGHT;
									else {
										if (SprintChargeLevel == SPRINT_CHARGE_MAX)
											ani = MARIO_ANI_SPRINT_FIRE_LEFT;
										else
											ani = MARIO_ANI_WALK_FIRE_LEFT;
									}
								}
							}
						}
					}
				}
			}
			break;
		}
		}
	}

	int alpha = 255;
	if (invuln == 1)
		alpha = 128;

	if ((form == MARIO_FORM_RACCOON) && (nx > 0) && (ani != MARIO_ANI_WARPING_RACCOON))
		animation_set->at(ani)->Render(x - 7.0f, y, alpha);
	else
		animation_set->at(ani)->Render(x, y, alpha);
	//RenderBoundingBox();
}

void CMario::SetPCForm(int form) {
	if (form == MARIO_FORM_SUPER)
		y--;

	this->form = form;
	if (IsAttacking) {
		IsAttacking = false;
		attack_start = 0;
	}
	if (IsFlying) {
		IsFlying = false;
		fly_start = 0;
	}
	if (IsFlapping) {
		IsFlapping = false;
		flap_start = 0;
	}
}

void CMario::SetState(int state) {
	CGameObject::SetState(state);

	switch (state) {
	case MARIO_STATE_WALK_RIGHT:
		if (IsChargingSprint)
			vx += MARIO_SPRINT_ACCELERATE_SPEED;
		else
			vx += MARIO_ACCELERATE_SPEED;
		nx = 1;
		break;
	case MARIO_STATE_WALK_LEFT:
		if (IsChargingSprint)
			vx -= MARIO_SPRINT_ACCELERATE_SPEED;
		else
			vx -= MARIO_ACCELERATE_SPEED;
		nx = -1;
		break;
	case MARIO_STATE_JUMP:
		IsUsingLift = false;
		grav_limit_x = grav_limit_X = 0;

		if (SprintChargeLevel == SPRINT_CHARGE_MAX) {
			vy = -MARIO_INIT_FLY_SPEED;
			if ((form == MARIO_FORM_RACCOON) && (!IsFlying)) {
				IsFlying = true;
				StartFly();
			}
		}
		else
			vy = -MARIO_JUMP_SPEED_Y;
		break;
	case MARIO_STATE_FLAP:
		if (IsFlying)
			vy = -MARIO_GRAVITY_FLY;
		else
			vy = MARIO_GRAVITY_FLY;
		IsFlapping = true;
		StartFlapping();
		break;
	case MARIO_STATE_DEATH:
		vx = 0;
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	case MARIO_STATE_ATTACK:
		IsAttacking = true;
		StartAttack();
		break;
	case MARIO_STATE_CROUCH:
		vx = 0;
		break;
	case MARIO_STATE_IDLE:
		if (nx > 0) {
			if (vx > 0)
				vx -= MARIO_ACCELERATE_SPEED;
			else
				vx = 0;
		}
		else {
			if (vx < 0)
				vx += MARIO_ACCELERATE_SPEED;
			else
				vx = 0;
		}
		break;
	case MARIO_STATE_EMERGING_DOWN:
		StartEmerge();
	case MARIO_STATE_WARPING_DOWN:
		vx = 0;
		vy = MARIO_WARP_SPEED;
		break;
	case MARIO_STATE_EMERGING_UP:
		StartEmerge();
	case MARIO_STATE_WARPING_UP:
		vx = 0;
		vy = -MARIO_WARP_SPEED;
		break;
	}
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom) {
	if ((state != MARIO_STATE_EMERGING_UP) && (state != MARIO_STATE_EMERGING_DOWN)) {
		left = x;
		top = y;
		if (form != MARIO_FORM_NORMAL) {
			if (state == MARIO_STATE_CROUCH) {
				right = x + MARIO_CROUCH_BBOX_WIDTH;
				bottom = y + MARIO_CROUCH_BBOX_HEIGHT;
			}
			else {
				right = x + MARIO_SUPER_BBOX_WIDTH;
				bottom = y + MARIO_SUPER_BBOX_HEIGHT;
			}
		}
		else {
			right = x + MARIO_NORMAL_BBOX_WIDTH;
			bottom = y + MARIO_NORMAL_BBOX_HEIGHT;
		}
	}
}