#include "circle.h"

Circle::Circle() {
	// Moveable();
	name = "Circle";
	addFlag(CIRCLE);
}

Circle::Circle(Vector2 loc, Colour col, float radius) {
	Circle();
	location = loc;
	colour = col;
	radius_ = radius;
}