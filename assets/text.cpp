#include "text.h"

Text::Text(Vector2 loc, Font* fon, Colour col, string con, float scl) : Text() {
	location = loc;
	font = fon;
	colour = col;
	default_colour = col;
	content = con;
	scale = scl;
	setDefaultColour(col);
}

Text::Text() {
	addFlag(TEXT | FIXED_POS);
}
