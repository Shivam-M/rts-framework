#pragma once

using namespace std;

#include <fstream>

#include "../tools/image.h"
#include "../defs/colour.h"

class Text;

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
	int* condition;
	int target;
	float speed = 0.1f;
};

enum ACTIONS {CLOSE_GAME, SWITCH_NATION, CHANGE_MAP_VIEW, CHANGE_CONTROLS, DEBUG, TEST_FONTS, TEST_FONTS2, OTHER};

class Moveable {
	public:
		string name = "Generic Moveable";
		Texture* texture = nullptr;
		Text* text = nullptr;
		vector<string> script;
		vector<Vector2> points;

		float script_timer = 0;
		int script_line = 0;
		int flags = ENABLED;
		ACTIONS button_action = OTHER;

		Vector2 acceleration;
		Vector2 velocity;
		Vector2 location;
		Vector2 text_offset;
		Vector2 size;

		ColourShift colour_shift;
		Colour colour = Colour("FFFFFF");
		Colour gradient_colour = colour;
		Colour default_colour = colour;

		bool shifting_colour = false, is_active = true, hover_tooltip = false;

		Moveable() {}
		Moveable(Vector2 loc, Vector2 sze, Colour col, Colour grd) : location(loc), size(sze), colour(col), gradient_colour(grd) {}

		virtual vector<Vector2> getPoints() { return points; }
		Text* getText() { return text; }
		Texture* getTexture() { return texture; }
		string getName() { return name; }
	
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

		int getFlags() { return flags; }
		void addFlag(int f) { flags |= f; }
		void removeFlag(int f) { flags &= ~f; }
		bool hasFlag(int f) { return flags & f; }

		void setText(Text* t) { text = t; }
		void setTextOffset(float x, float y);
		void setTexture(Texture* tex) { addFlag(TEXTURED); texture = tex; }
		void setName(string n) { name = n; }
		void setAcceleration(float x, float y) { acceleration.set(x, y); }
		void setVelocity(float x, float y) { velocity.set(x, y); }
		virtual void setLocation(float x, float y) { location.set(x, y); }
		void setSize(float x, float y) { size.set(x, y); }
		
		ACTIONS getButtonAction() { return button_action; }
		void setButtonAction(ACTIONS action) {
			addFlag(BUTTON);
			button_action = action;
		}

		void stopColourShift();
		void setColourShift(ColourShift col_shift) { colour_shift = col_shift; shifting_colour = true; }
		void setColour(Colour col) { colour = col; default_colour = col; }
		void setDefaultColour(Colour col) { default_colour = col; }
		void setGradientColour(Colour col) { gradient_colour = col; }
		void resetColour() { colour = default_colour; }
		void shiftColour();
		void tickTimer(float modifier);
		void loadScript(string script_path);
		
		// Change to new button class override
		virtual void onHover();
		
		virtual void onHoverStop();

		void common(float modifier);
		virtual void update(float modifier = 1.0);
};
