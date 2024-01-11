#pragma once
#include "../assets/moveable_new.h"

class CircleNew : public MoveableNew {
private:
	double generality_ = 1.0;
	double percentage_ = 0.00;

public:
	CircleNew();
	CircleNew(Vector2 location, Colour2 colour, double radius);

	double radius_;

	double getRadius() { return radius_; }
	double getPercentage() { return percentage_; }
	double getGenerality() { return generality_; }

	void setRadius(double radius) { radius_ = radius; } // Change size too.
	void setGenerality(double generality) { generality_ = generality; }
	void setPercentage(double percentage) { percentage_ = percentage; }

	// void update(double modifier = 1.0);
};
