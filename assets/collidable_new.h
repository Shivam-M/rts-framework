#pragma once

#include "moveable_new.h"

using namespace std;

class CollidableNew : public MoveableNew {
	public:
		bool can_collide = true;
		bool direction_right = true;
		double shuffle_size = 0;
		double shuffle_limit = 125;

		void update(double = 1.0);

		CollidableNew();
};