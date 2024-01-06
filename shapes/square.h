#ifndef SQUARE_H
#define SQUARE_H
#pragma once
#include "../assets/moveable.h"

class Square: public Moveable {
	public:
		Square(int, int, int, int, double, double, double, double, double = 1, bool = false);

		void stop();
		void update(double = 1.0);

	private:
		double shiftSpeed = 1;
};

#endif
