#include "fonts.h"


using namespace std;
using namespace glfreetype;

map<pair<string, int>, font_data*> Fonts::fonts = {};

void Fonts::loadFont(string font_location, int font_size, bool custom) {
	if (fonts.count({ font_location, font_size })) return;
	info("Loading font... " + font_location + " [size: " + to_string(font_size) + "]");
	font_data* font = new font_data();
	font->init(((custom ? "" : (string)DIRECTORY) + font_location).c_str(), font_size);
	fonts.insert(pair<pair<string, int>, font_data*>(make_pair(font_location, font_size), font));
}

font_data Fonts::getFont(string font_location, int font_size, bool custom) {
	loadFont(font_location, font_size, custom);
	return *fonts[make_pair(font_location, font_size)];
}
