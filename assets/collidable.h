#ifndef COLLIDABLE_H
#define COLLIDABLE_H
#pragma once
#include "moveable.h"
#include <map>
#include <vector>   

using namespace std;


class Collidable: public Moveable {
	public:
		bool canCollide = true;
		bool direction_right = true;
		double shuffle_size = 0.0;
		// double points[4][2] = { {0, 0}, {0, 0}, {0, 0}, {0, 0} };
		vector<map<double, double>> points;

		void update(double = 1.0);
		void setPoints(map<double, double>, map<double, double>, map<double, double>, map<double, double>);
		void setPoints(vector<map<double, double>>);

		vector<map<double, double>> getPoints();

		Collidable();
};

#endif
