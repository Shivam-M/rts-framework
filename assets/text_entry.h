#pragma once

#include "text.h"

#include <chrono>

using namespace std;

class TextEntry : public Text {
    private:
		const char CURSOR_CHAR = '|';
		const unsigned int CURSOR_FLASH_SPEED = 500;
        bool cursor_visible_ = true;
        string display_content_;

	public:
        TextEntry() = default;
		TextEntry(Vector2 location, Font* font, Colour colour, string content = "", float scale = 1.f) : Text(location, font, colour, content, scale) {}

		void input(const int& character) {
			if (character >= 255 and character != 259) return;

			if (character == 259) {
				if (content_.size() > 0) content_.pop_back();
			} else {
				content_.push_back(static_cast<char>(character));
			}
		}

        void flash_cursor() {
            static auto last_flash = chrono::steady_clock::now();
            auto now = chrono::steady_clock::now();
            if (chrono::duration_cast<chrono::milliseconds>(now - last_flash).count() > CURSOR_FLASH_SPEED) {
                cursor_visible_ = !cursor_visible_;
                last_flash = now;
            }
        }

        void update(float modifier = 1.0) override {
            Text::update(modifier);
            Text::common(modifier);
            flash_cursor();
        }

        const string& get_content(bool raw = false) override {
			if (raw) return content_;
            display_content_ = content_;
            if (cursor_visible_) 
                display_content_ += CURSOR_CHAR;
            return display_content_;
        }
};
