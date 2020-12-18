#include "TitleCursor.h"

void CTitleCursor::ChangeState() {
	if (state == CURSOR_SP) {
		state = CURSOR_MP;
		SetPosition(72, 160);
	}
	else {
		state = CURSOR_SP;
		SetPosition(72, 144);
	}
}