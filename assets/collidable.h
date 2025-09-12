#pragma once

#include "moveable.h"

using namespace std;

class Collidable : public Moveable {
	public:
		bool can_collide = true;
		bool direction_right = true;
		float shuffle_size = 0;
		float shuffle_limit = 125;

		void update(const float& modifier = 1.0) override {
			common(modifier);

			if (get_flags() & MOVING) {
				if (direction_right && shuffle_size >= shuffle_limit || !direction_right && shuffle_size <= -shuffle_limit) {
					direction_right = !direction_right;
				}

				if (direction_right) {
					velocity.x = 0.001 * modifier;
					shuffle_size += 1 * modifier;
				}
				else {
					velocity.x = -0.001 * modifier;
					shuffle_size -= 1 * modifier;
				}

				location.x += velocity.x;
				location.y += velocity.y;
			}
		}

		Collidable() { add_flag(COLLIDABLE); }
};