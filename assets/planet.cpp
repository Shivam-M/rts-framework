#include "planet.h"

/*
void Planet::setRadius(double r) {
	radius = r;
}*/

/*
void Planet::setCentre(double x, double y) {
	centre[0] = x; centre[1] = y;
	setLocation(x - getRadius(), y - getRadius());
}*/

void Planet::setGravity(double g) {
	gravity = g;
}

void Planet::setPull(double pf) {
	pull_factor = pf;
}
/*
void Planet::getCentre(double* x, double* y) {
	*x = centre[0];
	*y = centre[1];
	// memcpy(c, &centre, sizeof(double) * 2
}*/

/*
double Planet::getRadius() {
	return radius;
}*/

double Planet::getGravity() {
	return gravity;
}

bool Planet::withinVicinity(Moveable*) {
	return false;
}
