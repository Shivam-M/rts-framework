#include "text.h"

Text::Text(Vector2 loc, Font* fon, Colour col, string con, float scl) : Text() {
	location = loc;
	font = fon;
	colour = col;
	content = con;
	scale = scl;
}

Text::Text() {
	addFlag(TEXT);
	addFlag(FIXED_POS);
}