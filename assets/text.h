#pragma once

#include "../tools/text_renderer.h"
#include "moveable.h"

using namespace std;

enum ALIGNMENT { LEFT, CENTRE, RIGHT };

class Text: public Moveable {
	private:
		float scale = 1.0f;
		string content;
		Font* font;
		Vector2 text_size;
		ALIGNMENT alignment = LEFT;

	public:
		Text();
		Text(Vector2 location, Font* font, Colour colour, string content = "", float scale = 1.f);

		Font* getFont() { return font; }
		string& getContent() { return content; }
		float getScale() { return scale; }
		Vector2 getTextSize() { return text_size; }
		ALIGNMENT getAlignment() { return alignment; }
		Vector2 getLocation() {

			if (alignment == LEFT) {
				return Moveable::location;
			}
			else if (alignment == CENTRE) {
				return Moveable::location;
			}
			else if (alignment == RIGHT) {
				Vector2 temp_location = location;
				temp_location.x -= text_size.x / 1280.0;
				return temp_location;
			}
		}

		void updateCommon() { text_size = TextRenderer::calculate_text_dimensions(font, content, scale); }
		void setFont(Font* f) { font = f; updateCommon(); }
		void setContent(string c) { content = c; updateCommon(); }
		void setScale(float s) { scale = s; updateCommon(); }
		void setAlignment(ALIGNMENT a) { alignment = a; updateCommon(); }

};
