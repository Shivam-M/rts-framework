#ifndef ENEMY_H
#define ENEMY_H
#pragma once
#include "moveable.h"

class Enemy : public Moveable {
	private:
		bool stopping = false;
		bool direction_right = true;
		double shuffle_size = 0.0;
		bool facing_right_ = true;

	public:
		Enemy();
		void setAcceleration(double, double);
		void update(double = 1.0);
		void stop();

		bool facingRight();

		double max_speed = 0.00375;
};

#endif
