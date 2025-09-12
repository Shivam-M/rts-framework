#pragma once

#include "moveable.h"

#include <string>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

using namespace std;

struct Font;

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

		Font* get_font() { return font; }
		virtual string& get_content(bool raw = false) { return content; }
		const float& get_scale() { return scale; }
		Vector2 get_text_size() { return text_size; }
		ALIGNMENT get_alignment() { return alignment; }
		Vector2 get_location() {
			switch (alignment) {
				case LEFT:
					return Moveable::location;
				case CENTRE: {
					Vector2 temp = location;
					temp.x -= text_size.x / WINDOW_WIDTH / 2.0f;
					return temp;
				}
				case RIGHT: {
					Vector2 temp = location;
					temp.x -= text_size.x / WINDOW_WIDTH;
					return temp;
				}
				default:
					return Moveable::location;
				}
		}

		void update_common();
		void set_font(Font* f) { font = f; update_common(); }
		void set_content(const string& c) { content = c; update_common(); }
		void set_scale(float s) { scale = s; update_common(); }
		void set_alignment(ALIGNMENT a) { alignment = a; update_common(); }
};
