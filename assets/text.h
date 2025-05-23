#pragma once

#include "../tools/text_renderer.h"
#include "moveable.h"

using namespace std;

enum ALIGNMENT { LEFT, CENTRE, RIGHT };

class Text: public Moveable {
	private:
		float scale = 1.0f;
		Font* font;
		Vector2 text_size;
		ALIGNMENT alignment = LEFT;

	protected:
		string content;

	public:
		Text();
		Text(Vector2 location, Font* font, Colour colour, string content = "", float scale = 1.f);

		Font* getFont() { return font; }
		string& getContent() { return content; }
		const float& getScale() { return scale; }
		Vector2 getTextSize() { return text_size; }
		ALIGNMENT getAlignment() { return alignment; }
		Vector2 getLocation() {

			if (alignment == LEFT) {
				return Moveable::location;
			}
			else if (alignment == CENTRE) {
				Vector2 temp_location = location;
				temp_location.x -= text_size.x / WINDOW_WIDTH / 2;
				return temp_location;
			}
			else if (alignment == RIGHT) {
				Vector2 temp_location = location;
				temp_location.x -= text_size.x / WINDOW_WIDTH;
				return temp_location;
			}

			return Moveable::location;
		}

		void updateCommon() { text_size = TextRenderer::calculate_text_dimensions(font, content, scale); }
		void setFont(Font* f) { font = f; updateCommon(); }
		void setContent(const string& c) { content = c; updateCommon(); }
		void setScale(float s) { scale = s; updateCommon(); }
		void setAlignment(ALIGNMENT a) { alignment = a; updateCommon(); }

};
