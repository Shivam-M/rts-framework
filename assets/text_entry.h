#pragma once

#include "text.h"

using namespace std;

class TextEntry : public Text {
	private:
		char ASCII[256];  // TODO: only run once

	public:
		TextEntry() : Text() {
			for (int i = 0; i < 256; i++) {
				ASCII[i] = static_cast<char>(i);
			}
		}

		TextEntry(Vector2 location, Font* font, Colour colour, string content = "", float scale = 1.f) : Text( location, font, colour, content, scale) {
			for (int i = 0; i < 256; i++) {
				ASCII[i] = static_cast<char>(i);
			}
		}

		void input(const int& character) {
			if (character >= 255 and character != 259) return;

			if (character == 259) {
				if (content.size() > 0) content.pop_back();
			} else {
				content.push_back(ASCII[character]);
			}
		}

		void cursor_flash() {}

};
