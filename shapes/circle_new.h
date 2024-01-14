#pragma once
#include "../assets/moveable_new.h"

class Circle : public Moveable {
	private:
		float generality_ = 1.0;
		float percentage_ = 0.00;

	public:
		Circle();
		Circle(Vector2 location, Colour colour, float radius);

		float radius_;

		float getRadius() { return radius_; }
		float getPercentage() { return percentage_; }
		float getGenerality() { return generality_; }

		void setRadius(float radius) { radius_ = radius; } // Change size too.
		void setGenerality(float generality) { generality_ = generality; }
		void setPercentage(float percentage) { percentage_ = percentage; }

		// void update(double modifier = 1.0);
};
