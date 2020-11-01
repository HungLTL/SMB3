#include "Animations.h"
#include "Utility.h"

CAnimationSets* CAnimationSets::__instance = NULL;

void CAnimation::Add(int spriteID, DWORD time) {
	int t = time;
	if (time == 0)
		t = this->defaultTime;

	LPSPRITE sprite = CSprites::GetInstance()->Get(spriteID);
	LPANIMATION_FRAME frame = new CAnimationFrame(sprite, t);
	frames.push_back(frame);
}

void CAnimation::Render(float x, float y, int alpha) {
	DWORD now = GetTickCount();
	if (currentFrame == -1) {
		currentFrame = 0;
		lastFrameTime = now;
	}
	else {
		DWORD t = frames[currentFrame]->GetTime();
		if (now - lastFrameTime > t) {
			currentFrame++;
			lastFrameTime = now;
			if (currentFrame == frames.size())
				currentFrame = 0;
		}
	}
	frames[currentFrame]->GetSprite()->Draw(x, y, alpha);
}

CAnimations* CAnimations::__instance = NULL;

CAnimations* CAnimations::GetInstance() {
	if (__instance == NULL)
		__instance = new CAnimations();
	return __instance;
}

void CAnimations::Add(int id, LPANIMATION ani) {
	animations[id] = ani;
}

LPANIMATION CAnimations::Get(int id) {
	LPANIMATION ani = animations[id];
	return ani;
}

void CAnimations::Clear() {
	for (auto x : animations) {
		LPANIMATION ani = x.second;
		delete ani;
	}
	animations.clear();
}

CAnimationSets::CAnimationSets() {

}

CAnimationSets* CAnimationSets::GetInstance() {
	if (__instance == NULL)
		__instance = new CAnimationSets();
	return __instance;
}

LPANIMATION_SET CAnimationSets::Get(unsigned int id) {
	LPANIMATION_SET ani_set = animation_sets[id];
	return ani_set;
}

void CAnimationSets::Add(int id, LPANIMATION_SET ani_set) {
	animation_sets[id] = ani_set;
}