#pragma once

#include "moveable.h"

#include <string>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

using namespace std;

struct Font;

class Text: public Moveable {
	public:
		enum class Alignment { Left, Centre, Right };

	private:
		float scale_ = 1.0f;
		Font* font_ = nullptr;

	protected:
		string content_;

	public:
		Alignment alignment = Alignment::Left;
		Vector2 aligned_location;
		Vector2 dimensions;

		Text();
		Text(Vector2 location, Font* font, Colour colour, string content = "", float scale = 1.f);

		const Vector2& get_location() override;
		Font* get_font() { return font_; }
		virtual const string& get_content(bool raw = false) { return content_; }
		const float& get_scale() const { return scale_; }

		void set_font(Font* font);
		void set_content(const string& content);
		void set_scale(float scale);
		void update_common();
		void update(float modifier = 1.0) override;
};
