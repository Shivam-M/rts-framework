#include "text.h"

Text::Text(Vector2 loc, Font* fon, Colour col, string con) {
	addFlag(TEXT);
	location = loc;
	font = fon;
	colour = col;
	content = con;
}

Text::Text() {
	addFlag(TEXT);
}