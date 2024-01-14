#include "text_new.h"

Text::Text(Vector2 loc, font_data fon, Colour col, string con) {
	addFlag(TEXT);
	location = loc;
	font = fon;
	colour = col;
	content = con;
}

Text::Text() {
	addFlag(TEXT);
}