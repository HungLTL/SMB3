#include "CoinBlock.h"
#include "Coin.h"

#include "Game.h"
#include "PlayScene.h"

void CCoinBlock::Render() {
	int ani = -1;
	if (uses > 0)
		ani = CBLOCK_ANI;
	else
		ani = CBLOCK_ANI_EMPTY;
	animation_set->at(ani)->Render(x, y);
}

void CCoinBlock::SetStatus() {
	if (!IsNudging) {
		if (uses > 0) {
			IsNudging = true;
			uses--;
			vy = -BLOCK_SHIFT_SPEED;

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