#include "PowerBlock.h"

#include "Coin.h"
#include "PowerUp.h"

#include "Game.h"
#include "PlayScene.h"

void CPBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_object) {
	CGameObject::Update(dt);

	if (IsNudging)
		vy += BLOCK_RETRACT_SPEED;

	y += dy;

	if (y >= y0) {
		if (IsNudging) {
			IsNudging = false;

			if (type != COIN) {
				CPowerUp* powerup = NULL;
				switch (type) {
				case POWERUP: {
					CMario* player = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->getPlayer();
					if (player->GetPCForm() == MARIO_FORM_NORMAL) {
						if (ActivatedAtRightSide)
							powerup = new CPowerUp(POWERUP_TYPE_MUSHROOM, true);
						else
							powerup = new CPowerUp(POWERUP_TYPE_MUSHROOM);
					}
					else
						powerup = new CPowerUp(POWERUP_TYPE_LEAF);
					break;
				}
				case MUSHROOM_1UP: {
					if (type == MUSHROOM_1UP) {
						if (ActivatedAtRightSide)
							powerup = new CPowerUp(POWERUP_TYPE_1UP);
						else
							powerup = new CPowerUp(POWERUP_TYPE_1UP, true);
					}
					break;
				}
				case FLOWER:
					powerup = new CPowerUp(POWERUP_TYPE_FLOWER);
					break;
				}

				powerup->SetPosition(x0, y0);
				powerup->GetInitBoundaries();
				powerup->SetAnimationSet(CAnimationSets::GetInstance()->Get(16));
				dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->PushObject(powerup);
			}
		}
		y = y0;
		vy = 0;
	}
}


void CPBlock::Render() {
	int ani = 0;
	if (!IsUsed)
		ani = PBLOCK_ANI_UNUSED;
	else
		ani = PBLOCK_ANI_USED;
	animation_set->at(ani)->Render(x, y);
	//RenderBoundingBox();
}

void CPBlock::SetStatus() {
	if (!IsNudging) {
		IsNudging = true;
		vy = -BLOCK_SHIFT_SPEED;
		if (type == COIN) {
			CCoin* coin = new CCoin(1);
			coin->SetPosition(x0, y0);
			coin->GetInitBoundaries();
			coin->SetAnimationSet(CAnimationSets::GetInstance()->Get(15));
			dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->PushObject(coin);

			CGame::GetInstance()->AddCoin();
			CGame::GetInstance()->AddScore(100);
		}
	}
}