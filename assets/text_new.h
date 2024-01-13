#pragma once

#include "../tools/TextRenderer.hpp"
#include "moveable_new.h"

using namespace std;
using namespace glfreetype;

class TextNew: public MoveableNew {
	private:
		string content;
		font_data font;

	public:
		TextNew();
		TextNew(Vector2 location, font_data font, Colour2 colour, string content = "");

		font_data getFont() { return font; }
		string getContent() { return content; }

		void setFont(font_data f) { font = f; }
		void setContent(string c) { content = c; }
};
