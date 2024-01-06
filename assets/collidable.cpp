#include "Collidable.h"




Collidable::Collidable() {
	addFlag(COLLIDABLE);
}

vector<map<double, double>> Collidable::getPoints() { return points; }

void Collidable::update(double modifier) {
	common(modifier);

	if (getFlags() & MOVING) {

		if (direction_right && shuffle_size > 125) {
			direction_right = false;
		}
		else if (!direction_right && shuffle_size < -125) {
			direction_right = true;
		}

		if (direction_right) {
			velocity[0] = 0.001 * modifier;
			shuffle_size += 1 * modifier;
		}
		else {
			velocity[0] = -0.001 * modifier;
			shuffle_size -= 1 * modifier;
		}

		location[0] += velocity[0];
		location[1] += velocity[1];
	}
}

void Collidable::setPoints(map<double, double> a, map<double, double> b, map<double, double> c, map<double, double> d) {
	points.clear();
	points.push_back(a);
	points.push_back(b);
	points.push_back(c);
	points.push_back(d);
}

void Collidable::setPoints(vector<map<double, double>> p) {
	points = p;
}
