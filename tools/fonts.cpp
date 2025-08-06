#include "fonts.h"

#include "../tools/common.h"
#include "../tools/text_renderer.h"

map<pair<string, int>, Font*> Fonts::fonts = {};
map<pair<string, int>, bool> Fonts::fonts_is_custom_temp = {};

void Fonts::loadFont(const string& font_location, int font_size, bool custom) {
	if (fonts.count({ font_location, font_size })) return;
	log_t("Loading font... " CON_RED + font_location + CON_NORMAL " [size: " CON_RED + to_string(font_size) + CON_NORMAL "]");
	Font* font = TextRenderer::load_font((custom ? "" : DIRECTORY) + font_location, font_size);
	if (!font) {
		log_t("Using fallback font " CON_RED FALLBACK_FONT);
		font = TextRenderer::load_font(FALLBACK_FONT, font_size);
	}
	fonts.insert(pair<pair<string, int>, Font*>(make_pair(font_location, font_size), font)); // need extra for scale
	fonts_is_custom_temp.insert(pair<pair<string, int>, bool>(make_pair(font_location, font_size), custom)); // need extra for scale
}

Font* Fonts::getFont(const string& font_location, int font_size, bool custom) {
	loadFont(font_location, font_size, custom);
	return fonts[make_pair(font_location, font_size)];
}

pair<string, int> Fonts::getProperties(Font* font) { // Temp
	for (const auto& entry : fonts) {
		if (entry.second == font) {
			return { entry.first.first, fonts_is_custom_temp[entry.first] };
		}
	}
	return { "", -1 };
}