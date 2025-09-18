#include "text.h"

#include "../tools/text_renderer.h"

Text::Text(Vector2 loc, Font* font, Colour col, string content, float scale) : Text() {
	location = loc;
	font_ = font;
	colour = col;
	content_ = content;
	scale_ = scale;
}

Text::Text() {
	add_flag(TEXT | FIXED_POS);
}

const Vector2& Text::get_location() {
	return aligned_location;
}

void Text::set_font(Font* font) {
	font_ = font;
	update_common();
}

void Text::set_content(const string& content) {
	content_ = content;
	update_common();
}

void Text::set_scale(float scale) {
	scale_ = scale;
	update_common();
}

void Text::update_common() {
	dimensions = TextRenderer::calculate_text_dimensions(font_, content_, scale_);
}

void Text::update(float modifier) {
	Moveable::update(modifier);

	aligned_location = location;
	switch (alignment) {
		case Alignment::Centre:
			aligned_location.x -= dimensions.x / WINDOW_WIDTH / 2.0f;
			break;
		case Alignment::Right:
			aligned_location.x -= dimensions.x / WINDOW_WIDTH;
			break;
	}
}