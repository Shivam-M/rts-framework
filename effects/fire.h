#ifndef FIRE_H
#define FIRE_H
#pragma once
#include "../shapes/square.h"
#include <vector>

class Fire: public Moveable {
	private:
		int count = 0;

	public:
		std::vector<Moveable*>* objects;

		Fire();
		void stop();
		void update(double = 1.0);
};

#endif
