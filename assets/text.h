#pragma once

#include "../tools/text_renderer.h"
#include "moveable.h"

using namespace std;

class Text: public Moveable {
	private:
		float scale = 1.0f;
		string content;
		Font* font;
		Vector2 text_size;

	public:
		Text();
		Text(Vector2 location, Font* font, Colour colour, string content = "", float scale = 1.f);

		Font* getFont() { return font; }
		string& getContent() { return content; }
		float getScale() { return scale; }
		Vector2 getTextSize() { return text_size; }

		void setFont(Font* f) { font = f; text_size = TextRenderer::calculate_text_dimensions(font, content, scale); }
		void setContent(string c) { content = c; text_size = TextRenderer::calculate_text_dimensions(font, content, scale); }
		void setScale(float s) { scale = s; text_size = TextRenderer::calculate_text_dimensions(font, content, scale); }
};
