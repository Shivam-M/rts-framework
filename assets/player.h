#pragma once

#include "moveable.h"

class Player: public Moveable {
	private:
		float const MAX_JUMP = 200;
		float MAX_SPEED = 0.00375f;

		float rotation_ = 0.f;
		float jump_size_ = MAX_JUMP;
		float jump_power_ = 0.009f;
		float jump_time_ = 4;
		bool in_jump_cooldown_ = false;
		bool is_jumping_ = false;
		bool is_stopping = false;
		bool is_facing_right_ = true;

	public:
		virtual ~Player() {};
		Player();

		void replenish_jump();
		void jump();
		void set_acceleration(float x, float y) override;
		void stop();
		void default_physics(float);
		void update(float modifier = 1.0) override;
};