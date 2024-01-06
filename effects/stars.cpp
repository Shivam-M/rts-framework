#include "stars.h"


Stars::Stars() {
	name = "Stars";
	setSize(0.1, 0.1);
	setColour(255, 0, 0, 0);
}

void Stars::update(double modifier) {
	common(modifier);
	if (particle_counter % max(1, (int)(1 / modifier)) == 0) {
		// Circle* s = new Circle(location[0] + (size[0] * ((double)rand() / (RAND_MAX))), location[1] + (size[1] * ((double)rand() / (RAND_MAX))), 0.0025, 255, 255, 255, 255);

		// Square is 7x faster to render than circle... 
		// Can lower line loop value from 360 depending on quality.

		Square* s = new Square(255, 255, 255, 255, 0.005 / 2, 0.005, location[0] + (size[0] * ((double)rand() / (RAND_MAX))), location[1] + (size[1] * ((double)rand() / (RAND_MAX))), 0.01, true);

		// s->shiftSpeed = 0.01;
		s->oneWay = true;
		// s->addFlag(BACKGROUND);
		s->addFlag(STARS);
		s->setAlphaShifting(true, true);
		
		s->name = "Star Particle";

		Circle* c = new Circle(location[0] + (size[0] * ((double)rand() / (RAND_MAX))), location[1] + (size[1] * ((double)rand() / (RAND_MAX))), 0.0025 * ((double)rand() / (RAND_MAX)), 255, 255, 255, 255);
		c->addFlag(STARS);
		c->oneWay = true;
		c->setGenerality(45);
		c->setColour(162, 155, 254, 200);
		c->setColour(255, 255, 255, 200);
		c->setGradientColour(255, 255, 255, 0);
		c->setAlphaShifting(true, true);
		// if render method 1

		
		c->updateMatrix();
		objects->push_back(c);

		double r = (double)rand() / (RAND_MAX);
		if (r < 0.33)		 s->setColour(236, 240, 241, 175);
		else if (r < 0.66)	 s->setColour(189, 195, 199, 175);
		else				 s->setColour(223, 230, 233, 175);
		// objects->push_back(s);
		delete s; // temp
	}
	particle_counter++;
}
