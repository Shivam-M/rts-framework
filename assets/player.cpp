#include "player.h"

const auto PI = acos(-1);

Player::Player() {
	name = "Player";
	add_flag(PLAYER);
}

void Player::jump() {
	if (is_jumping_ || in_jump_cooldown_) return;
	is_jumping_ = true; // 0-single
}

void Player::set_acceleration(float x, float y) {
	is_stopping = false;
	acceleration = { x, y };
	is_facing_right_ = true ? acceleration.x >= 0 : false;
}

void Player::stop() {
	is_stopping = true;
	acceleration.x = 0;
}

void Player::replenish_jump() {
	in_jump_cooldown_ = true;
	jump_time_ = 50; // 0.5s
}

void Player::default_physics(float modifier) {
	if (is_stopping == true) {
		velocity.x *= 1 - (0.05 * (modifier));
		if (abs(velocity.x) <= 0.001) {
			velocity.x = 0;
			is_stopping = false;
		}
	}

	rotation_ = -90;

	if (is_jumping_) {
		velocity.y = -jump_power_;
		jump_size_ -= 10 * modifier;

		rotation_ = (360 * jump_size_ / MAX_JUMP);
		if (!is_facing_right_) rotation_ = 360 - rotation_;
	}
}


void Player::update(float modifier) {
	common(modifier);

	default_physics(modifier);

	if (is_jumping_) {
		if (jump_size_ <= 0) {
			is_jumping_ = false;
			jump_size_ = MAX_JUMP;
			replenish_jump();
		} else if (jump_size_ <= MAX_JUMP / 16) {
			velocity.y = 0;
		}
	}

	if (in_jump_cooldown_) {
		jump_time_ -= modifier;
		if (jump_time_ <= 0) {
			in_jump_cooldown_ = false;
			rotation_ = 0;
		}
	}

	if ((acceleration.x >= 0 && velocity.x < MAX_SPEED) || (acceleration.x <= 0 && velocity.x > -MAX_SPEED))
		velocity.x += acceleration.x * modifier;
	
	velocity.y += acceleration.y * modifier;

	location.x += velocity.x * modifier;
	location.y += velocity.y * modifier;
}
