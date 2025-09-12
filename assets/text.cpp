#include "text.h"

#include "../tools/text_renderer.h"

Text::Text(Vector2 loc, Font* fon, Colour col, string con, float scl) : Text() {
	location = loc;
	font = fon;
	colour = col;
	default_colour = col;
	content = con;
	scale = scl;
	set_default_colour(col);
}

Text::Text() {
	add_flag(TEXT | FIXED_POS);
}

void Text::update_common() {
	text_size = TextRenderer::calculate_text_dimensions(font, content, scale);
}
