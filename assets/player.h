#pragma once
#include "moveable.h"

class Player: public Moveable {
	private:
		bool stopping = false;
		bool is_jumping_ = false;
		float const MAX_JUMP = 200;
		float jump_size_ = MAX_JUMP;
		float jump_power_ = 0.009f;
		float jump_time_ = 4;
		bool jump_cooldown_ = false;
		bool facing_right_ = true;

		Vector2 pr_j_location;
		Vector2 j_velocity;
		Vector2 pre_loc = { 0, 0 };

		float rotation_inc = 0;

	public:
		Player();

		bool facingRight();
		bool isJumping();
		void replenishJump();
		void executeJump();
		void setAcceleration(float, float) override;
		void update(const float& = 1.0) override;
		void stop();
		void increaseRotation(float);
		void stopRotation();

		void defaultPhysics(float);

		float max_speed = 0.00375f; //* 2;// *10;
		float rotation = 0.f;
};