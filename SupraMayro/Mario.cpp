#include <algorithm>
#include <assert.h>
#include "Mario.h"
#include "Utility.h"
#include "PlayScene.h"
#include "Game.h"
#include "GoldBlock.h"
#include "PowerBlock.h"
#include "Fireball.h"
#include "Goomba.h"
#include "Koopa.h"

bool CMario::GetGravStatus() {
	if (IsGrounded == true)
		return true;
	else
		return false;
}

bool CMario::GetAttackStatus() {
	if (IsAttacking)
		return true;
	else
		return false;
}

bool CMario::GetCarryStatus() {
	if (IsCarrying)
		return true;
	else
		return false;
}

CMario::CMario(float x, float y) :CGameObject() {
	form = MARIO_FORM_NORMAL;
	SetState(MARIO_STATE_IDLE);

	IsGrounded = true;
	IsCarrying = false;

	IsAttacking = false;
	AttackTime = 0;
	invuln = 0;

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
			if (dynamic_cast<CFireball*>(e->obj)) {
				delete e;
				continue;
			}

			if ((dynamic_cast<CBackgroundPlatform*>(e->obj))|| (dynamic_cast<CGoomba*>(e->obj))||(dynamic_cast<CKoopa*>(e->obj))) {
				if (dynamic_cast<CBackgroundPlatform*>(e->obj)) {
					if (e->ny < 0)
						coEvents.push_back(e);
				}
				if ((dynamic_cast<CGoomba*>(e->obj))||(dynamic_cast<CKoopa*>(e->obj))) {
					if (invuln == 0) {
						if (dynamic_cast<CGoomba*>(e->obj))
							coEvents.push_back(e);
						else {
							CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
							if (koopa->GetState() == KOOPA_STATE_DORMANT) {
								if (!IsCarrying)
									coEvents.push_back(e);
							}
							else
								coEvents.push_back(e);
						}
					}
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

	if (vy > 0)
		IsGrounded = false;

	if (IsAttacking) {
		AttackTime++;
	}
	switch (form) {
	case MARIO_FORM_RACCOON:
		if (AttackTime == 21) {
			AttackTime = 0;
			IsAttacking = false;
		}
		break;
	case MARIO_FORM_FIRE:
		if (AttackTime == 13) {
			AttackTime = 0;
			IsAttacking = false;
		}
		break;
	}

	vy += MARIO_GRAVITY * dt;

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
				if (e->ny < 0)
					IsGrounded = true;
			}
			if (dynamic_cast<CPBlock*>(e->obj)) {
				CPBlock* PowerBlock = dynamic_cast<CPBlock*>(e->obj);
				if (e->ny > 0) {
					if (!PowerBlock->GetState())
						PowerBlock->SetState(true);
				}
			}
			if ((form == MARIO_FORM_RACCOON)&&(AttackTime==20)) {
				if (dynamic_cast<CGoldBlock*>(e->obj)) {
					CGoldBlock* block = dynamic_cast<CGoldBlock*>(e->obj);
					if ((e->nx != 0) || (e->ny > 0))
						block->ChangeState();
				}
			}
			if (dynamic_cast<CGoomba*>(e->obj))
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

				if (e->ny < 0)
				{
					if (goomba->GetState() == GOOMBA_STATE_WALK)
					{
						goomba->SetState(GOOMBA_STATE_FLATTENED);
						vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
				}
				else if (e->nx != 0)
				{
					if (invuln == 0)
					{
						if (goomba->GetState() == GOOMBA_STATE_WALK)
						{
							switch (form) {
							case MARIO_FORM_RACCOON:
								if (IsAttacking) {
									goomba->SetState(GOOMBA_STATE_DEATH);
								}
								else {
									form = MARIO_FORM_SUPER;
									StartInvuln();
								}
								break;
							case MARIO_FORM_FIRE:
								form = MARIO_FORM_SUPER;
								StartInvuln();
								break;
							case MARIO_FORM_SUPER:
								form = MARIO_FORM_NORMAL;
								StartInvuln();
								break;
							case MARIO_FORM_NORMAL:
								SetState(MARIO_STATE_DEATH);
							}
						}
					}
				}
			}
			if (dynamic_cast<CKoopa*>(e->obj)) {
				CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);

				if (koopa->GetState() == KOOPA_STATE_DORMANT) {
					if ((e->ny > 0) || (e->nx != 0)) {
						if (!IsCarrying) {
							StartInvuln();
							if (this->nx > 0)
								koopa->SetState(KOOPA_STATE_PINBALL_RIGHT);
							else
								koopa->SetState(KOOPA_STATE_PINBALL_LEFT);
							if (e->ny > 0)
								vy = -MARIO_JUMP_DEFLECT_SPEED;
						}
					}
				}

				if (e->ny > 0) {
					if ((koopa->GetState() == KOOPA_STATE_WALK_LEFT) || (koopa->GetState() == KOOPA_STATE_WALK_RIGHT)) {
						koopa->SetState(KOOPA_STATE_DORMANT);
						vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
				}
				else if (e->nx != 0)
				{
					if (invuln == 0)
					{
						if ((koopa->GetState() != KOOPA_STATE_DEATH)&&(koopa->GetState() != KOOPA_STATE_DORMANT))
						{
							switch (form) {
							case MARIO_FORM_RACCOON:
								if (IsAttacking) {
									koopa->SetState(KOOPA_STATE_DEATH);
								}
								else {
									form = MARIO_FORM_SUPER;
									StartInvuln();
								}
								break;
							case MARIO_FORM_FIRE:
								form = MARIO_FORM_SUPER;
								StartInvuln();
								break;
							case MARIO_FORM_SUPER:
								form = MARIO_FORM_NORMAL;
								StartInvuln();
								break;
							case MARIO_FORM_NORMAL:
								SetState(MARIO_STATE_DEATH);
							}
						}
					}
				}
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
			if (!IsGrounded) {
				if (nx > 0)
					ani = MARIO_ANI_JUMP_NORMAL_RIGHT;
				else
					ani = MARIO_ANI_JUMP_NORMAL_LEFT;
			}
			else {
				if (vx == 0) {
					if (nx > 0)
						ani = MARIO_ANI_IDLE_NORMAL_RIGHT;
					else
						ani = MARIO_ANI_IDLE_NORMAL_LEFT;
				}
				else {
					switch (state) {
					case MARIO_STATE_WALK_LEFT:
						ani = MARIO_ANI_WALK_NORMAL_LEFT;
						break;
					case MARIO_STATE_WALK_RIGHT:
						ani = MARIO_ANI_WALK_NORMAL_RIGHT;
						break;
					case MARIO_STATE_SPRINT_LEFT:
						ani = MARIO_ANI_SPRINT_NORMAL_LEFT;
						break;
					case MARIO_STATE_SPRINT_RIGHT:
						ani = MARIO_ANI_SPRINT_NORMAL_RIGHT;
						break;
					case MARIO_STATE_IDLE: {
						if (nx > 0)
							ani = MARIO_ANI_IDLE_NORMAL_RIGHT;
						else
							ani = MARIO_ANI_IDLE_NORMAL_LEFT;
					}
					}
				}	
			}
			break;
		}
		case MARIO_FORM_SUPER: {
			if (!IsGrounded) {
				if (nx > 0)
					ani = MARIO_ANI_JUMP_SUPER_RIGHT;
				else
					ani = MARIO_ANI_JUMP_SUPER_LEFT;
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
					switch (state) {
					case MARIO_STATE_WALK_LEFT:
						ani = MARIO_ANI_WALK_SUPER_LEFT;
						break;
					case MARIO_STATE_WALK_RIGHT:
						ani = MARIO_ANI_WALK_SUPER_RIGHT;
						break;
					case MARIO_STATE_SPRINT_LEFT:
						ani = MARIO_ANI_SPRINT_SUPER_LEFT;
						break;
					case MARIO_STATE_SPRINT_RIGHT:
						ani = MARIO_ANI_SPRINT_SUPER_RIGHT;
						break;
					case MARIO_STATE_IDLE: {
						if (nx > 0)
							ani = MARIO_ANI_IDLE_SUPER_RIGHT;
						else
							ani = MARIO_ANI_IDLE_SUPER_LEFT;
					}
					}
				}
			}
			break;
		}
		case MARIO_FORM_RACCOON: {
			if (!IsGrounded) {
				if (state == MARIO_STATE_FLY) {
					if (nx > 0)
						ani = MARIO_ANI_FLY_RACCOON_RIGHT;
					else
						ani = MARIO_ANI_FLY_RACCOON_LEFT;
				}
				else {
					if (nx > 0)
						ani = MARIO_ANI_JUMP_RACCOON_RIGHT;
					else
						ani = MARIO_ANI_JUMP_RACCOON_LEFT;
				}
			}
			else {
				if (IsAttacking) {
					if (nx > 0)
						ani = MARIO_ANI_ATTACK_RACCOON_RIGHT;
					else
						ani = MARIO_ANI_ATTACK_RACCOON_LEFT;
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
						switch (state) {
						case MARIO_STATE_WALK_LEFT:
							ani = MARIO_ANI_WALK_RACCOON_LEFT;
							break;
						case MARIO_STATE_WALK_RIGHT:
							ani = MARIO_ANI_WALK_RACCOON_RIGHT;
							break;
						case MARIO_STATE_SPRINT_LEFT:
							ani = MARIO_ANI_SPRINT_RACCOON_LEFT;
							break;
						case MARIO_STATE_SPRINT_RIGHT:
							ani = MARIO_ANI_SPRINT_RACCOON_RIGHT;
							break;
						case MARIO_STATE_FLY: {
							if (nx > 0)
								ani = MARIO_ANI_WALK_RACCOON_RIGHT;
							else
								ani = MARIO_ANI_WALK_RACCOON_LEFT;
							break;
						}
						case MARIO_STATE_IDLE: {
							if (nx > 0)
								ani = MARIO_ANI_IDLE_RACCOON_RIGHT;
							else
								ani = MARIO_ANI_IDLE_RACCOON_LEFT;
							break;
						}
						}
					}
				}
			}
			break;
		}
		case MARIO_FORM_FIRE: {
			{
				if (!IsGrounded) {
					if (nx > 0)
						ani = MARIO_ANI_JUMP_FIRE_RIGHT;
					else
						ani = MARIO_ANI_JUMP_FIRE_LEFT;
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
							switch (state) {
							case MARIO_STATE_WALK_LEFT:
								ani = MARIO_ANI_WALK_FIRE_LEFT;
								break;
							case MARIO_STATE_WALK_RIGHT:
								ani = MARIO_ANI_WALK_FIRE_RIGHT;
								break;
							case MARIO_STATE_SPRINT_LEFT:
								ani = MARIO_ANI_SPRINT_FIRE_LEFT;
								break;
							case MARIO_STATE_SPRINT_RIGHT:
								ani = MARIO_ANI_SPRINT_FIRE_RIGHT;
								break;
							case MARIO_STATE_IDLE: {
								if (nx > 0)
									ani = MARIO_ANI_IDLE_FIRE_RIGHT;
								else
									ani = MARIO_ANI_IDLE_FIRE_LEFT;
							}
							}
						}
					}
				}
				break;
			}
		}
		}
	}

	if ((form == MARIO_FORM_RACCOON) && (nx > 0))
		animation_set->at(ani)->Render(x - 7, y);
	else
		animation_set->at(ani)->Render(x, y);
	//RenderBoundingBox();
}

void CMario::SetState(int state) {
	CGameObject::SetState(state);

	switch (state) {
	case MARIO_STATE_WALK_RIGHT:
		if (vx < MARIO_MAX_WALKING_SPEED)
			vx += MARIO_ACCELERATE_SPEED;
		nx = 1;
		break;
	case MARIO_STATE_WALK_LEFT:
		if (vx > -MARIO_MAX_WALKING_SPEED)
			vx -= MARIO_ACCELERATE_SPEED;
		nx = -1;
		break;
	case MARIO_STATE_SPRINT_RIGHT:
		if (vx <= MARIO_MAX_WALKING_SPEED * 2)
			vx += MARIO_SPRINT_ACCELERATE_SPEED;
		nx = 1;
		break;
	case MARIO_STATE_SPRINT_LEFT:
		if (vx > -MARIO_MAX_WALKING_SPEED * 2)
			vx -= MARIO_SPRINT_ACCELERATE_SPEED;
		nx = -1;
		break;
	case MARIO_STATE_FLY:
		if (vy < 0) {
			if (((vx >= MARIO_MAX_WALKING_SPEED * 2) && (nx > 0)) || ((vx <= -MARIO_MAX_WALKING_SPEED * 2) && (nx < 0)))
				vy = -MARIO_JUMP_SPEED_Y*0.5;
		}
		if (vy > 0) {
			vy = MARIO_GRAVITY_FLY;
		}
		if (vx == 0)
			this->SetState(MARIO_STATE_IDLE);
		break;
	case MARIO_STATE_JUMP:
		if (IsGrounded) {
			IsGrounded = false;
			vy = -MARIO_JUMP_SPEED_Y;	
		}
		break;
	case MARIO_STATE_DEATH:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	case MARIO_STATE_ATTACK: {
		if (!IsAttacking)
			IsAttacking = true;
		break;
	}
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
	}
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom) {
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