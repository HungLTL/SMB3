#include "BackgroundObjectRect.h"

void CBackgroundObjectRect::Render() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < length; j++) {
			animation_set->at(0)->Render(x + (j * WIDTH), y + (i * WIDTH));
		}
	}
}