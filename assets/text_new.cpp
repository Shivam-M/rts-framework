#include "text_new.h"

TextNew::TextNew(Vector2 loc, font_data fon, Colour2 col, string con) {
	addFlag(TEXT);
	location = loc;
	font = fon;
	colour = col;
	content = con;
}

TextNew::TextNew() {
	addFlag(TEXT);
}