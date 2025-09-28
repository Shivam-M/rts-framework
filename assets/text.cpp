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
	TextRenderer::calculate_text_dimensions(font_, content_, scale_, dimensions);
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

	if (!background_) return;

	const Colour BACKGROUND_COLOUR = Colour(40, 40, 40, 200);
	const float BACKGROUND_SCALE = 1.05f;

	Vector2 text_dimensions = dimensions / Vector2(1280, 720);
	Vector2 text_location = get_location();

	text_location.y -= text_dimensions.y;
	text_dimensions *= {BACKGROUND_SCALE, BACKGROUND_SCALE * (16.0 / 9.0)};

	text_location.x -= text_dimensions.x * (BACKGROUND_SCALE - 1) / (2 * BACKGROUND_SCALE);
	text_location.y -= text_dimensions.y * (BACKGROUND_SCALE * 1.5 - 1) / (2 * BACKGROUND_SCALE * 1.5);

	background_->set_colour(BACKGROUND_COLOUR);
	background_->set_gradient_colour(BACKGROUND_COLOUR);
	background_->location = text_location;
	background_->size = text_dimensions;
}