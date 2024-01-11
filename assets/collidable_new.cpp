#include "collidable_new.h"

CollidableNew::CollidableNew() {
	addFlag(COLLIDABLE);
}

void CollidableNew::update(double modifier) {
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

		location.x += velocity.y;
		location.x += velocity.y;
	}
}
