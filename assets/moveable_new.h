#pragma once

using namespace std;

#include <fstream>

#include "../tools/image.h"
#include "../defs/colour.h"

class TextNew;

struct ColourShift {
	ColourShift(Colour2 first, Colour2 second) : first_colour(first), second_colour(second) {}
	enum DIRECTION { UP, DOWN };
	DIRECTION direction = DOWN;
	Colour2 first_colour, second_colour;
	bool loop = true, fade_to_death = false;
	float speed = 1.0;
	short alpha_limits[2] = { 0, 255 };
};

class MoveableNew {
	public:
		string name = "Generic Moveable";
		Texture* texture;
		TextNew* text;
		vector<string> script;
		vector<Vector2> points;

		float script_timer = 0;
		int script_line = 0;
		int flags = ENABLED;

		Vector2 acceleration;
		Vector2 velocity;
		Vector2 location;
		Vector2 text_offset;
		Vector2 size;

		Colour2 colour = Colour2("FFFFFF");
		Colour2 alternate_colour = colour;
		Colour2 gradient_colour = colour;
		Colour2 shift_colour = colour;
		Colour2 default_colour = colour;
		Colour2 target_colour = Colour2("000000");

		short alpha_max = 255, alpha_min = 0;
		bool shifting_colour = false, shifting_downwards = true, oneWay = false, isActive = true;

		MoveableNew() {}
		MoveableNew(Vector2 loc, Vector2 sze, Colour2 col, Colour2 grd) : location(loc), size(sze), colour(col), gradient_colour(col) {}

		virtual vector<Vector2> getPoints() { return points; }
		TextNew* getText() { return text; }
		Texture* getTexture() { return texture; }
		string getName() { return name; }
	
		Vector2 getAcceleration() { return acceleration; }
		Vector2 getVelocity() { return velocity; }
		Vector2 getLocation() { return location; }
		Vector2 getSize() { return size; }
		Vector2 getCentre() { return Vector2(location.x + size.x / 2, location.y + size.y / 2); }
		Vector2 getTextOffset() { return text_offset; }

		Colour2 getColour() { return colour; }
		Colour2 getGradientColour() { return gradient_colour; }

		int getFlags() { return flags; }
		void addFlag(int f) { flags |= f; }
		void removeFlag(int f) { flags &= ~f; }

		void setText(TextNew* t) { text = t; }
		void setTextOffset(float x, float y) { text_offset.set(x, y); }
		void setTexture(Texture* tex) { addFlag(TEXTURED); texture = tex; }
		void setName(string n) { name = n; }
		void setAcceleration(float x, float y) { acceleration.set(x, y); }
		void setVelocity(float x, float y) { velocity.set(x, y); }
		virtual void setLocation(float x, float y) { location.set(x, y); }
		virtual void setSize(float x, float y) { size.set(x, y); }

		void setColour(Colour2 col) { colour = col; }
		void setDefaultColour(Colour2 col) { alternate_colour = col; }
		void setAlternateColour(Colour2 col) { default_colour = col; }
		void setGradientColour(Colour2 col) { gradient_colour = col; }
		void resetColour() { colour = default_colour; }
		void tickTimer(float modifier);

		void setColourShifting(bool state) { shifting_colour = state; }
		void setDownwardsShifting(bool state) { shifting_downwards = state; }
		void shiftColour(float speed = 0.01);

		void loadScript(string script_path);

		void common(float);
		virtual void update(float = 1.0);
};
