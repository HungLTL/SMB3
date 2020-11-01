#include "Sprites.h"
#include "Game.h"

CSprite::CSprite(int id, int left, int top, int right, int bottom, LPDIRECT3DTEXTURE9 tex) {
	this->id = id;
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
	this->texture = tex;
}

CSprites* CSprites::__instance = NULL;

CSprites* CSprites::GetInstance() {
	if (__instance == NULL)
		__instance = new CSprites();
	return __instance;
}

void CSprite::Draw(float x, float y, int alpha) {
	CGame* game = CGame::GetInstance();
	game->Draw(x, y, texture, left, top, right, bottom, alpha);
}

void CSprites::Add(int id, int left, int top, int right, int bottom, LPDIRECT3DTEXTURE9 tex) {
	LPSPRITE s = new CSprite(id, left, top, right, bottom, tex);
	sprites[id] = s;
}

LPSPRITE CSprites::Get(int id) {
	return sprites[id];
}

void CSprites::Clear() {
	for (auto x : sprites) {
		LPSPRITE s = x.second;
		delete s;
	}
	sprites.clear();
}