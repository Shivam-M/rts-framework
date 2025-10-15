#pragma once

#include "text.h"

#include <chrono>

using namespace std;
using namespace chrono;

class TextEntry : public Text {
    private:
		inline static constexpr char CURSOR_CHAR = '|';
        inline static constexpr unsigned short CURSOR_FLASH_SPEED = 500;

        bool cursor_visible_ = true;
        string display_content_;
        steady_clock::time_point last_flash_ = steady_clock::now();

	public:
        virtual ~TextEntry() {}
        TextEntry() : Text() {}
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
            auto now = steady_clock::now();
            if (duration_cast<milliseconds>(now - last_flash_).count() > CURSOR_FLASH_SPEED) {
                cursor_visible_ = !cursor_visible_;
                last_flash_ = now;
            }
        }

        void update(float modifier = 1.0) override {
            Text::update(modifier);
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
