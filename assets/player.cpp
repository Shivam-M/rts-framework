#include "player.h"

const auto PI = acos(-1);

Player::Player() {
	name = "Player";
	addFlag(PLAYER);
	/*
	planet = new Planet();
	planet->setCentre(0.45, 0.5);
	planet->setRadius(0.1);
	planet->addFlag(CIRCLE);
	planet->setGradientColour(22, 160, 133, 0);
	planet->setColour(26, 188, 156, 220);*/
}

bool Player::isJumping() {
	return is_jumping_;
}

void Player::executeJump() {
	if (is_jumping_ || jump_cooldown_) return;
	is_jumping_ = true; //0-single
	pr_j_location[0] = location[0];
	pr_j_location[1] = location[1];
}

void Player::setAcceleration(double x, double y) {
	stopping = false;
	acceleration[0] = x;
	acceleration[1] = y;
	facing_right_ = true ? acceleration[0] >= 0 : false;
}

void Player::stop() {
	stopping = true;
	acceleration[0] = 0;
}

void Player::increaseRotation(double amount) {
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

void Player::hookPlanet(Planet* p) {
	planet = p;
}

Planet* Player::getPlanet() {
	return planet;
}

void Player::switchMode(int m) {
	is_jumping_ = false;
	jump_size_ = MAX_JUMP;
	replenishJump();
	mode = m;
	if (mode == DEFAULT) rotation = 0;
}

void Player::defaultPhysics(double modifier) {
	if (stopping == true) {
		velocity[0] *= 1 - (0.05 * (modifier));
		if (abs(velocity[0]) <= 0.001) {
			velocity[0] = 0;
			stopping = false;
		}
	}

	rotation = -90;

	if (isJumping()) {
		velocity[1] = -jump_power_;
		jump_size_ -= 10 * modifier;

		rotation = (360 * jump_size_ / MAX_JUMP);
		if (!facingRight()) rotation = 360 - rotation;
	}
}

void Player::planetPhysics(double modifier) {
	rotation += rotation_inc;
	if (rotation >= 360) rotation = 0;
	if (rotation < 0) rotation = 359;

	double circle_centre[2]{};
	planet->getCentre(&circle_centre[0], &circle_centre[1]);

	double ext = size[1] / 2 / 2;

	location[0] = circle_centre[0] + cos(rotation * (PI / 180)) * (planet->radius_+ ext);
	location[1] = circle_centre[1] + (sin(rotation * (PI / 180)) * (planet->radius_+ ext) * 16.0 / 9); // CHANGE!

	if (isJumping()) {
		jump_size_ -= 5 * modifier;
		planet->radius_ += (jump_size_ < MAX_JUMP / 2) ? -0.005 : 0.005;
	}
}

void Player::update(double modifier) {
	common(modifier);

	if (mode == DEFAULT) defaultPhysics(modifier);
	else planetPhysics(modifier);

	if (isJumping()) {
		if (jump_size_ <= 0) {
			is_jumping_ = false;
			jump_size_ = MAX_JUMP;
			replenishJump();
		} else if (jump_size_ <= MAX_JUMP / 16) {
			velocity[1] = 0;
		}
	}

	if (jump_cooldown_) {
		jump_time_ -= modifier;
		if (jump_time_ <= 0) {
			jump_cooldown_ = false;
			if (mode != DEFAULT) return;
			rotation = 0;
		}
	}

	if ((acceleration[0] >= 0 && velocity[0] < max_speed) || (acceleration[0] <= 0 && velocity[0] > -max_speed))
		velocity[0] += acceleration[0] * modifier;
	
	velocity[1] += acceleration[1] * modifier;

	location[0] += velocity[0] * modifier;
	location[1] += velocity[1] * modifier;
}
