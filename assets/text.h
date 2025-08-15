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

		Font* getFont() { return font; }
		virtual string& getContent(bool raw = false) { return content; }
		const float& getScale() { return scale; }
		Vector2 getTextSize() { return text_size; }
		ALIGNMENT getAlignment() { return alignment; }
		Vector2 getLocation() {
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

		void updateCommon();
		void setFont(Font* f) { font = f; updateCommon(); }
		void setContent(const string& c) { content = c; updateCommon(); }
		void setScale(float s) { scale = s; updateCommon(); }
		void setAlignment(ALIGNMENT a) { alignment = a; updateCommon(); }
};
