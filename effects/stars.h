#ifndef STARS_H
#define STARS_H
#pragma once
#include "../shapes/circle.h"
#include "../shapes/square.h"
#include <vector>

class Stars : public Moveable {
	private:
		int particle_counter = 0;

	public:
		std::vector<Moveable*>* objects;

		Stars();
		void stop();
		void update(double = 1.0);
};

#endif
