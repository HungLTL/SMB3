#include "BackgroundObjectHeight.h"
#include "Game.h"

void CBackgroundObjectHeight::Render() {
	for (int i = 0; i < height; i++)
		animation_set->at(0)->Render(x, y + (i * WIDTH));
}