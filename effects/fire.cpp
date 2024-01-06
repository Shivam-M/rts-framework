#include "fire.h"



Fire::Fire() {
	name = "Fire";
	setSize(0.1, 0.1);
	setColour(255, 0, 0, 0);
	setVelocity(0.001, 0);
}

void Fire::update(double modifier) {
	common(modifier);

	for (int x = 0; x < 1; x++) {
		Square* s = new Square(255, 255, 255, 255, 0.01, 0.01, location[0] + (size[0] * ((double)rand() / (RAND_MAX))), location[1] + (size[1] * ((double)rand() / (RAND_MAX))), 0.05 * modifier, true);
		
		double r = (double)rand() / (RAND_MAX);
		if (r < 0.33) {
			s->setColour(241, 196, 15, 175);
			// s->setColour(155, 89, 182);
			// s->setColour(231, 76, 60);
			// s->setGradientColour(243, 156, 18);
		} else if (r < 0.66) {
			s->setColour(243, 156, 18, 175);
			// s->setColour(26, 188, 156);
			// s->setColour(192, 57, 43);
			// s->setGradientColour(231, 76, 60);
		} else {
			s->setColour(231, 76, 60, 175);
			// s->setColour(236, 240, 241);
			// s->setGradientColour(241, 196, 15);
		} objects->push_back(s);
	}
	
	velocity[0] += acceleration[0];
	velocity[1] += acceleration[1];
	location[0] += velocity[0];
	location[1] += velocity[1];
	count++;
}
