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
	TOOLTIP =			1 << 23,
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
	bool enabled;

	ColourFilter(const Colour& c, const Mode& m, const bool& e = true) : colour(c), mode(m), enabled(e) {}
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

enum Action {PauseGame, SwitchNation, ToggleMapView, ToggleControls, SaveGame, DebugFonts, ToggleTooltip, HireUnit, ToggleDebugUI, DeclareWar, Debug, Other, PauseSimulation};

class Moveable {
	private:
		int flags_ = ENABLED;
		bool shifting_colour_ = false;
		int script_line_ = 0;
		float script_timer_ = 0;
		vector<string>* script = nullptr;
		Action button_action_ = Other;
		Vector2 text_offset_;
		Texture* texture_ = nullptr;
		Texture* secondary_texture_ = nullptr;

		void tick_timer(float modifier);

	public:
		string name = "Generic Moveable";
		Text* text = nullptr;
		vector<Vector2> points;
		string metadata = "";
		Moveable* parent = nullptr;
		Blend blend;

		Vector2 acceleration;
		Vector2 velocity;
		Vector2 location;
		Vector2 size;

		Colour colour = COLOUR_WHITE;
		Colour gradient_colour = COLOUR_NULL;
		Colour evaluated_colour = Colour(100, 255, 100);
		ColourShift colour_shift;
		ColourFilter hover_filter = ColourFilter(Colour(1, 1, 1, 0.75), ColourFilter::Mode::Multiplication, false);
		deque<ColourFilter*> filters;

		bool is_active = true;

		Moveable() {}
		Moveable(Vector2 loc, Vector2 sze, Colour col, Colour grd) : location(loc), size(sze), colour(col), gradient_colour(grd) {}

		vector<Vector2> get_points() const { return points; }
		Texture* get_texture() { return texture_; }
		Texture* get_secondary_texture() { return secondary_texture_; }
	
		
		virtual Vector2 get_centre() { return Vector2(location.x + size.x / 2, location.y + size.y / 2); }
		const Vector2& get_velocity() const { return velocity; }
		const Vector2& get_size() const { return size; }
		const Vector2& get_acceleration() const { return acceleration; }
		virtual const Vector2& get_location() { return location; }
		virtual void set_size(float x, float y) { size = { x, y }; }
		virtual void set_acceleration(float x, float y) { acceleration = { x, y }; }
		virtual void set_location(float x, float y) { location = { x, y }; }

		Colour get_colour() const { return colour; }
		const Colour& get_evaluated_colour() { return evaluated_colour; }
		void stop_colour_shift();
		void set_colour_shift(const ColourShift& col_shift);
		void shift_colour(float modifier = 1.0f);
		void set_colour(const Colour& col) { colour = col; }
		void set_gradient_colour(const Colour& gradient) { gradient_colour = gradient; }

		const int& get_flags() const { return flags_; }
		void add_flag(int flag) { flags_ |= flag; }
		void remove_flag(int flag) { flags_ &= ~flag; }
		bool has_flag(int flag) const { return flags_ & flag; }

		void set_text_offset(float x, float y);
		void set_texture(Texture* texture) { add_flag(TEXTURED); texture_ = texture; }
		void set_secondary_texture(Texture* secondary_texture) { add_flag(TEXTURED); secondary_texture_ = secondary_texture; }
		void set_velocity(float x, float y) { velocity = { x, y }; }
		
		const Action& get_button_action() const { return button_action_; }
		void set_button_action(Action action) { add_flag(BUTTON); button_action_ = action; }

		void load_script(const string& script_path);
		
		virtual void on_hover();
		virtual void on_hover_stop();

		void common(float modifier);
		virtual void update(float modifier = 1.0);
};
