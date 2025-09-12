#include "player.h"

const auto PI = acos(-1);

Player::Player() {
	name = "Player";
	add_flag(PLAYER);
}

bool Player::is_jumping() {
	return is_jumping_;
}

void Player::jump() {
	if (is_jumping_ || jump_cooldown_) return;
	is_jumping_ = true; //0-single
	pr_j_location = location;
}

void Player::set_acceleration(float x, float y) {
	stopping = false;
	acceleration = { x, y };
	facing_right_ = true ? acceleration.x >= 0 : false;
}

void Player::stop() {
	stopping = true;
	acceleration.x = 0;
}

void Player::increase_rotation(float amount) {
	rotation_inc = amount;
}

void Player::stop_rotation() {
	rotation_inc = 0;
}

void Player::replenish_jump() {
	jump_cooldown_ = true;
	jump_time_ = 50; // 0.5s
}

bool Player::is_facing_right() {
	return facing_right_;
}

void Player::default_physics(float modifier) {
	if (stopping == true) {
		velocity.x *= 1 - (0.05 * (modifier));
		if (abs(velocity.x) <= 0.001) {
			velocity.x = 0;
			stopping = false;
		}
	}

	rotation = -90;

	if (is_jumping()) {
		velocity.y = -jump_power_;
		jump_size_ -= 10 * modifier;

		rotation = (360 * jump_size_ / MAX_JUMP);
		if (!is_facing_right()) rotation = 360 - rotation;
	}
}


void Player::update(const float& modifier) {
	common(modifier);

	default_physics(modifier);

	if (is_jumping()) {
		if (jump_size_ <= 0) {
			is_jumping_ = false;
			jump_size_ = MAX_JUMP;
			replenish_jump();
		} else if (jump_size_ <= MAX_JUMP / 16) {
			velocity.y = 0;
		}
	}

	if (jump_cooldown_) {
		jump_time_ -= modifier;
		if (jump_time_ <= 0) {
			jump_cooldown_ = false;
			rotation = 0;
		}
	}

	if ((acceleration.x >= 0 && velocity.x < max_speed) || (acceleration.x <= 0 && velocity.x > -max_speed))
		velocity.x += acceleration.x * modifier;
	
	velocity.y += acceleration.y * modifier;

	location.x += velocity.x * modifier;
	location.y += velocity.y * modifier;
}
