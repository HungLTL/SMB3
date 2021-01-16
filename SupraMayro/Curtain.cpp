#include "Curtain.h"

#include "TitleCursor.h"

#include "Game.h"
#include "IntroScene.h"

CCurtain::CCurtain(int height):CBackgroundObjectHeight(height) {
	vector<int> vec(height, CURTAIN_BODY);
	vec[height - 1] = CURTAIN_EDGE;
	data = vec;
	unveil_start = 0;

	StartUnveil();
}

void CCurtain::Update(DWORD dt) {
	if (GetTickCount() - unveil_start >= CURTAIN_OPENING_TIME) {
		float dy = -CURTAIN_UNVEIL_SPEED * dt;
		y += dy;
		unveil_start = 0;
	}
	
	if (y + height * WIDTH <= 17) {
		y = 17.0f - (float)(height * WIDTH);

		if (dynamic_cast<CIntroScene*>(CGame::GetInstance()->GetCurrentScene())->getCursor() == NULL) {
			CTitleCursor* cursor = new CTitleCursor();
			cursor->SetPosition(72, 144);

			CAnimationSets* animation_sets = CAnimationSets::GetInstance();
			cursor->SetAnimationSet(animation_sets->Get(2));
			(dynamic_cast<CIntroScene*>(CGame::GetInstance()->GetCurrentScene()))->SetCursor(cursor);
		}
	}
	
}

void CCurtain::Render() {
	for (int i = 0; i < height; i++)
		animation_set->at(data[i])->Render(x, y + (i * WIDTH));
}