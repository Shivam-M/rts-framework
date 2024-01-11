#ifndef COLLIDABLE_NEW_H
#define COLLIDABLE_NEW_H
#pragma once

#include "moveable_new.h"
#include <vector>   

using namespace std;

class CollidableNew : public MoveableNew {
	public:
		bool can_collide = true;
		bool direction_right = true;
		double shuffle_size = 0;
		double shuffle_limit = 125;

		void update(double = 1.0);

		CollidableNew();
};

#endif