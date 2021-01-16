#include "Shrub.h"
#include "Game.h"

CShrub::CShrub(vector<int> info) {
	for (size_t i = 0; i < info.size(); i++)
		data.push_back(info[i]);
}

void CShrub::Render() {
	for (size_t i = 0; i < data.size(); i++) {
		if (data[i] == 12)
			continue;
		animation_set->at(data[i])->Render(x + (i * WIDTH), y);
	}
}