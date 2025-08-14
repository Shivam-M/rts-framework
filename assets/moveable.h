#pragma once

#include "colour.h"

#include <vector>
#include <string>

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

// TODO: Replace with new Blend value
struct ColourShift {
	ColourShift() {};
	ColourShift(Colour first, Colour second) : first_colour(first), second_colour(second) {
		if (first < second) swap(first_colour, second_colour);
	}
	void setCondition(int* value) {
		condition = value;
		target = *value;
	}
	void reswap() { swap(first_colour, second_colour); }
	enum DIRECTION { UP, DOWN };
	DIRECTION direction = DOWN;
	Colour first_colour, second_colour;
	bool loop = true, fade_to_death = false, with_gradient = false;
	int* condition = nullptr;
	int target = -1;
	float speed = 0.1f;
};

struct Blend {
	Blend() {}
	Blend(int typ, float spd, float sze, Vector2 dir, bool fxd) : type(typ), speed(spd), size(sze), direction(dir), fixed(fxd) {}
	int type = 0;
	float speed = 1.0f;
	float size = 1.0f;
	bool fixed = false;
	Vector2 direction = Vector2(0.0f, 0.0f);
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
		Colour gradient_colour = COLOUR_NULL;
		Colour default_colour = colour;

		bool shifting_colour = false, is_active = true, hover_tooltip = false;

		Moveable() {}
		Moveable(Vector2 loc, Vector2 sze, Colour col, Colour grd) : location(loc), size(sze), colour(col), gradient_colour(grd) {}

		const float& getPriority() { return priority; }
		virtual vector<Vector2> getPoints() { return points; }
		Text* getText() { return text; }
		Texture* getTexture() { return texture; }
		Texture* getSecondaryTexture() { return secondary_texture; }
		string& getName() { return name; }
	
		Vector2 getAcceleration() { return acceleration; }
		Vector2 getVelocity() { return velocity; }
		Vector2 getLocation() { return location; }
		Vector2 getSize() { return size; }
		Vector2 getCentre() { return Vector2(location.x + size.x / 2, location.y + size.y / 2); }
		Vector2 getTextOffset() { return text_offset; }

		Colour& getColourRef() { return colour; }
		Colour getColour() { return colour; }
		Colour& getGradientColourRef() { return gradient_colour; }
		Colour getGradientColour() { return gradient_colour; }
		Colour getDefaultColour() { return default_colour; }

		Blend& getBlend() { return blend; }

		const int& getFlags() { return flags; }
		void addFlag(const int& f) { flags |= f; }
		void removeFlag(const int& f) { flags &= ~f; }
		bool hasFlag(const int& f) { return flags & f; }

		void setText(Text* t) { text = t; }
		void setTextOffset(float x, float y);
		void setTexture(Texture* tex) { addFlag(TEXTURED); texture = tex; }
		void setSecondaryTexture(Texture* tex) { addFlag(TEXTURED); secondary_texture = tex; }
		void setName(string n) { name = n; }
		void setPriority(float p) { priority = p; }
		virtual void setAcceleration(float x, float y) { acceleration.set(x, y); }
		virtual void setVelocity(float x, float y) { velocity.set(x, y); }
		virtual void setLocation(float x, float y) { location.set(x, y); }
		virtual void setSize(float x, float y) { size.set(x, y); }
		
		BUTTON_ACTION getButtonAction() { return button_action; }
		void setButtonAction(BUTTON_ACTION action) { addFlag(BUTTON); button_action = action; }

		void stopColourShift();
		void setColourShift(ColourShift col_shift) { colour_shift = col_shift; shifting_colour = true; }
		void setColour(Colour col) { colour = col; default_colour = col; }
		void setDefaultColour(Colour col) { default_colour = col; }
		void setGradientColour(Colour col) { gradient_colour = col; }
		void resetColour() { colour = default_colour; }
		void shiftColour();
		void setBlend(Blend b) { blend = b; }
		void tickTimer(const float& modifier);
		void loadScript(string script_path);
		
		// Change to new button class override
		virtual void onHover();
		virtual void onHoverStop();

		void common(const float& modifier);
		virtual void update(const float& modifier = 1.0);
};
