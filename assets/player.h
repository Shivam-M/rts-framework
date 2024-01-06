#ifndef PLAYER_H
#define PLAYER_H
#pragma once
#include "moveable.h"
#include "planet.h"

class Player: public Moveable {
	private:
		bool stopping = false;
		bool is_jumping_ = false;
		double const MAX_JUMP = 200;
		double jump_size_ = MAX_JUMP;
		double jump_power_ = 0.009;
		double jump_time_ = 4;
		bool jump_cooldown_ = false;
		bool facing_right_ = true;
		

		double pr_j_location[2] = { 0, 0 };
		double j_velocity[2] = {0, 0};

		double pre_loc[2] = { 0, 0 };


		double radius = 0.1;

		double rotation_inc = 0;


	public:
		Player();

		int mode = DEFAULT;

		bool facingRight();
		bool isJumping();
		void replenishJump();
		void executeJump();
		void setAcceleration(double, double);
		void update(double = 1.0);
		void stop();
		void increaseRotation(double);
		void stopRotation();

		Planet* getPlanet();

		void switchMode(int m);

		void defaultPhysics(double);
		void planetPhysics(double);

		void hookPlanet(Planet*);

		double max_speed = 0.00375; //* 2;// *10;
		double rotation = 0;

		Planet* planet = nullptr;
};

#endif
