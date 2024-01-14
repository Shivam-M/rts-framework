#include "collidable_new.h"

Collidable::Collidable() {
	addFlag(COLLIDABLE);
}

void Collidable::update(float modifier) {
	common(modifier);

	if (getFlags() & MOVING) {
		if (direction_right && shuffle_size >= shuffle_limit || !direction_right && shuffle_size <= 0) {
			direction_right != direction_right;
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
