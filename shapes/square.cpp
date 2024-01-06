#include "square.h"


Square::Square(int r, int g, int b, int a, double w, double h, double x, double y, double s, bool o) {
	setColour(r, g, b, a);
	setSize(w, h);
	setLocation(x, y);
	shiftSpeed = s;
	oneWay = o;
	name = "Square";
	addFlag(SQUARE);
}

void Square::stop() {
	// free(this);
}

void Square::update(double modifier) {
	common(modifier);


	if (colour[3] <= 0 && oneWay) {
		// printf("Turning off ALPHASHIFT\n");
		setAlphaShifting(false, true);
		isActive = false;
		// delete this;
		// free and cleanup later...
	}
}
