#pragma once

#include "moveable.h"

using namespace std;

class Collidable : public Moveable {
	public:
		bool can_collide = true;
		bool direction_right = true;
		float shuffle_size = 0;
		float shuffle_limit = 125;

		void update(float = 1.0);

		Collidable();
};