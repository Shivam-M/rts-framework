#pragma once

#include "colour.h"

#include <vector>
#include <string>
#include <deque>

using namespace std;

class Text;
struct Texture;

// Flag definitions
enum {
    BACKGROUND =        1 << 0,
    COLLIDABLE =        1 << 1,
    UNEDITABLE =        1 << 2,
    TEXT =              1 << 3,
    ENABLED =           1 << 4,
    PLAYER =            1 << 5,
    MOVING =            1 << 6,
    BUTTON =            1 << 7,
    SELECTED =          1 << 8,
    QUAD =              1 << 9,
    TEXTURED =          1 << 10,
    CURVED =            1 << 11,
    DISABLED =          1 << 12,
    FIXED_POS =         1 << 13,
    BOUND =             1 << 14,
    PROVINCE =          1 << 15,
    UNIT =              1 << 16,
    NATION =            1 << 17,
    CUSTOM =            1 << 18,
    PANEL =             1 << 19,
    PARTICLES =         1 << 20,
    TEXT_BACKGROUND =   1 << 21,
    THEMED =            1 << 22,
    UNSAVEABLE =        1 << 24,
    UI =                1 << 25,
    UNDER_PANEL =       1 << 26,
    DRAGGABLE =         1 << 27,
    NO_RENDER =         1 << 28
};

struct ColourFilter {
	enum class Mode { Addition, Multiplication, Replacement };
	Mode mode;
	Colour colour;

	ColourFilter(const Colour& c, const Mode& m) : colour(c), mode(m) {}
};

struct ColourShift {
	enum class Direction { Up, Down };
	Direction direction = Direction::Down;
	Colour first, second, evaluated;
	float speed = 0.1f;
	float progress = 0.0f;
	int* condition = nullptr;
	int target = -1;
	bool loop = true, fade_to_death = false, with_gradient = false;

	ColourShift() = default;
	ColourShift(const Colour& f, const Colour& s) : first(f), second(s) {}

	void conditionalise(int* value) {
		condition = value;
		target = *value;
	}

	const Colour& evaluate() {
		evaluated = first * (1.0f - progress) + second * progress;
		return evaluated;
	}
};

struct Blend {
	int type = 0;
	bool fixed = false;
	float speed = 1.0f;
	float size = 1.0f;
	Vector2 direction;

	Blend() = default;
	Blend(int typ, float spd, float sze, Vector2 dir, bool fxd) : type(typ), speed(spd), size(sze), direction(dir), fixed(fxd) {}
};

enum BUTTON_ACTION {PAUSE_GAME, SWITCH_NATION, CHANGE_MAP_VIEW, CHANGE_CONTROLS, SAVE_GAME, TEST_FONTS, TOGGLE_TOOLTIP, HIRE_UNIT, UI_DEBUG_TOGGLE, DECLARE_WAR, DEBUG, OTHER, PAUSE_SIMULATION};

class Moveable {
	public:
		string name = "Generic Moveable";
		Texture* texture = nullptr;
		Texture* secondary_texture = nullptr;
		Text* text = nullptr;
		vector<string>* script = nullptr;
		vector<Vector2> points;
		float priority = 0.0f;
		string metadata = "";
		Moveable* parent = nullptr;
		Blend blend;

		float script_timer = 0;
		int script_line = 0;
		int flags = ENABLED;
		BUTTON_ACTION button_action = OTHER;

		Vector2 acceleration;
		Vector2 velocity;
		Vector2 location;
		Vector2 text_offset;
		Vector2 size;

		ColourShift colour_shift;
		Colour colour = Colour("FFFFFF");
		deque<ColourFilter*> filters;
		Colour gradient_colour = COLOUR_NULL;
		Colour evaluated_colour = COLOUR_NULL;
		Colour default_colour = colour;

		bool shifting_colour = false, is_active = true, hover_tooltip = false;

		Moveable() {}
		Moveable(Vector2 loc, Vector2 sze, Colour col, Colour grd) : location(loc), size(sze), colour(col), gradient_colour(grd) {}

		const float& get_priority() { return priority; }
		virtual vector<Vector2> get_points() { return points; }
		Text* get_text() { return text; }
		Texture* get_texture() { return texture; }
		Texture* get_secondary_texture() { return secondary_texture; }
		string& get_name() { return name; }
	
		Vector2 get_acceleration() { return acceleration; }
		Vector2 get_velocity() { return velocity; }
		Vector2 get_location() { return location; }
		Vector2 get_size() { return size; }
		Vector2 get_centre() { return Vector2(location.x + size.x / 2, location.y + size.y / 2); }
		Vector2 get_text_offset() { return text_offset; }

		Colour& get_colour_ref() { return colour; }
		Colour get_colour() { return colour; }
		Colour& get_evaluated_colour() { return evaluated_colour; }
		Colour& get_gradient_colour_ref() { return gradient_colour; }
		Colour* get_gradient_colour_p() { 
			if (gradient_colour == colour or gradient_colour == COLOUR_NULL) return nullptr;
			else return &gradient_colour;
		}
		Colour get_gradient_colour() { return gradient_colour; }
		Colour get_default_colour() { return default_colour; }

		Blend& get_blend() { return blend; }

		const int& get_flags() { return flags; }
		void add_flag(const int& f) { flags |= f; }
		void remove_flag(const int& f) { flags &= ~f; }
		bool has_flag(const int& f) { return flags & f; }

		void set_text(Text* t) { text = t; }
		void set_text_offset(float x, float y);
		void set_texture(Texture* tex) { add_flag(TEXTURED); texture = tex; }
		void set_secondary_texture(Texture* tex) { add_flag(TEXTURED); secondary_texture = tex; }
		void set_name(string n) { name = n; }
		void set_priority(float p) { priority = p; }
		virtual void set_acceleration(float x, float y) { acceleration.set(x, y); }
		virtual void set_velocity(float x, float y) { velocity.set(x, y); }
		virtual void set_location(float x, float y) { location.set(x, y); }
		virtual void set_size(float x, float y) { size.set(x, y); }
		
		BUTTON_ACTION get_button_action() { return button_action; }
		void set_button_action(BUTTON_ACTION action) { add_flag(BUTTON); button_action = action; }

		void stop_colour_shift();
		void set_colour_shift(ColourShift col_shift) {
			colour_shift = col_shift;
			shifting_colour = true;
			// filters.emplace_front(new ColourFilter(colour_shift.evaluate(), ColourFilter::Mode::Replacement));
		}
		void set_colour(Colour col) { colour = col; default_colour = col; }
		void set_default_colour(Colour col) { default_colour = col; }
		void set_gradient_colour(Colour col) { gradient_colour = col; }
		void reset_colour() { colour = default_colour; }
		void shift_colour();
		void set_blend(Blend b) { blend = b; }
		void tick_timer(const float& modifier);
		void load_script(string script_path);
		
		// Change to new button class override
		virtual void on_hover();
		virtual void on_hover_stop();

		void common(const float& modifier);
		virtual void update(const float& modifier = 1.0);
};
