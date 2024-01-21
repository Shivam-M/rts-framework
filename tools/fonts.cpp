#include "fonts.h"

using namespace std;

map<pair<string, int>, Font*> Fonts::fonts = {};

void Fonts::loadFont(string font_location, int font_size, bool custom) {
	if (fonts.count({ font_location, font_size })) return;
	log_t("Loading font... " CON_RED + font_location + CON_NORMAL " [size: " CON_RED + to_string(font_size) + CON_NORMAL "]");
	Font* font = TextRenderer::load_font(custom ? "" : DIRECTORY + font_location, font_size);
	fonts.insert(pair<pair<string, int>, Font*>(make_pair(font_location, font_size), font));
}

Font* Fonts::getFont(string font_location, int font_size, bool custom) {
	loadFont(font_location, font_size, custom);
	return fonts[make_pair(font_location, font_size)];
}

