#pragma once

#include "moveable.h"

#include <string>
#include <cstdint>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

using namespace std;

struct Font;

class Text: public Moveable {
	public:
		enum class Alignment : uint8_t { Left, Centre, Right };

	private:
		Font* font_ = nullptr;
		Moveable* background_ = nullptr;

	protected:
		string content_;
		float scale_ = 1.0f;

	public:
		Alignment alignment = Alignment::Left;
		Vector2 aligned_location;
		Vector2 dimensions;

		virtual ~Text() {};
		Text();
		Text(Vector2 location, Font* font, Colour colour, string content = "", float scale = 1.f);

		Moveable* get_background() { return background_; }
		const Vector2& get_location() override;
		Font* get_font() { return font_; }
		virtual const string& get_content(bool raw = false) { return content_; }
		const float& get_scale() const { return scale_; }

		void set_background(Moveable* background) { add_flag(TEXT_BACKGROUND); background_ = background; }
		void set_font(Font* font);
		void set_content(const string& content);
		void set_scale(float scale);
		void update_common();
		void update(float modifier = 1.0) override;
};
