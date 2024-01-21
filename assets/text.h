#pragma once

#include "../tools/text_renderer.h"
#include "moveable.h"

using namespace std;
class Text: public Moveable {
	private:
		string content;
		Font* font;

	public:
		Text();
		Text(Vector2 location, Font* font, Colour colour, string content = "");

		Font* getFont() { return font; }
		string getContent() { return content; }

		void setFont(Font* f) { font = f; }
		void setContent(string c) { content = c; }
};
