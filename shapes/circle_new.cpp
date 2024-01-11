#include "circle_new.h"

CircleNew::CircleNew() {
	// MoveableNew();
	name = "Circle";
	addFlag(CIRCLE);
}

CircleNew::CircleNew(Vector2 loc, Colour2 col, double radius) {
	CircleNew();
	location = loc;
	colour = col;
	radius_ = radius;
}