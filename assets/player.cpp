#include "player.h"

const auto PI = acos(-1);

Player::Player() {
	name = "Player";
	addFlag(PLAYER);
}

bool Player::isJumping() {
	return is_jumping_;
}

void Player::executeJump() {
	if (is_jumping_ || jump_cooldown_) return;
	is_jumping_ = true; //0-single
	pr_j_location = location;
}

void Player::setAcceleration(float x, float y) {
	stopping = false;
	acceleration = { x, y };
	facing_right_ = true ? acceleration.x >= 0 : false;
}

void Player::stop() {
	stopping = true;
	acceleration.x = 0;
}

void Player::increaseRotation(float amount) {
	rotation_inc = amount;
}

void Player::stopRotation() {
	rotation_inc = 0;
}

void Player::replenishJump() {
	jump_cooldown_ = true;
	jump_time_ = 50; // 0.5s
}

bool Player::facingRight() {
	return facing_right_;
}

void Player::defaultPhysics(float modifier) {
	if (stopping == true) {
		velocity.x *= 1 - (0.05 * (modifier));
		if (abs(velocity.x) <= 0.001) {
			velocity.x = 0;
			stopping = false;
		}
	}

	rotation = -90;

	if (isJumping()) {
		velocity.y = -jump_power_;
		jump_size_ -= 10 * modifier;

		rotation = (360 * jump_size_ / MAX_JUMP);
		if (!facingRight()) rotation = 360 - rotation;
	}
}


void Player::update(float modifier) {
	common(modifier);

	defaultPhysics(modifier);

	if (isJumping()) {
		if (jump_size_ <= 0) {
			is_jumping_ = false;
			jump_size_ = MAX_JUMP;
			replenishJump();
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
