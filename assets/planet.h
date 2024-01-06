#ifndef PLANET_H
#define PLANET_H
#pragma once
#include "../shapes/circle.h"

class Planet: public Circle {
	private:
		// double centre[2] = {0.1, 0.1};
		// double radius = 0.2;
		double gravity = 0.001;
		double pull_factor = 0.1;

	public:
		// void setRadius(double);
		// void setCentre(double, double);
		void setGravity(double);
		void setPull(double);

		// void getCentre(double*, double*);

		// double getRadius();
		double getGravity();

		// double radius = 0.2; // temp

		bool withinVicinity(Moveable*);
};

#endif