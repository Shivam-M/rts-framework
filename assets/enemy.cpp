#include "enemy.h"


Enemy::Enemy() {
	name = "Enemy";
	addFlag(ENEMY);
	setColour(214, 48, 49, 173);
}

void Enemy::setAcceleration(double x, double y) {
	stopping = false;
	acceleration[0] = x;
	acceleration[1] = y;
}

void Enemy::stop() {
	stopping = true;
	acceleration[0] = 0;
}

bool Enemy::facingRight() {
	return facing_right_;
}

void Enemy::update(double modifier) {
	common(modifier);

	if (direction_right && shuffle_size > 50) {
		direction_right = false;
		facing_right_ = true;
	}
	else if (!direction_right && shuffle_size < -50) {
		direction_right = true;
		facing_right_ = false;
	}

	if (direction_right) {
		acceleration[0] = 0.0001 * modifier;
		shuffle_size += 1 * modifier;
	}
	else {
		acceleration[0] = -0.0001 * modifier;
		shuffle_size -= 1 * modifier;
	}

	if (stopping == true) {
		velocity[0] *= 0.95;
		if (std::abs(velocity[0]) <= 0.001) {
			velocity[0] = 0;
			stopping = false;
		}
	}
	if ((acceleration[0] >= 0 && velocity[0] < max_speed * modifier) || (acceleration[0] <= 0 && velocity[0] > -max_speed * modifier)) {
		velocity[0] += acceleration[0];
		velocity[1] += acceleration[1];
	}
	location[0] += velocity[0];
	location[1] += velocity[1];
}
