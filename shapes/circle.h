#ifndef CIRCLE_H
#define CIRCLE_H
#pragma once
#include "../assets/moveable.h"
#include "glm/glm.hpp"

class Circle: public Moveable {
	private:
		// double radius_;
		double generality_ = 1.0;
		double percentage_ = 0.00;
		double centre[2] = { 0, 0 };
		glm::mat4 matrix = glm::mat4(1.f);

	public:
		Circle();
		Circle(double, double, double, int, int, int, int);

		double shiftSpeed = 0.01;
		double radius_;

		void update(double = 1.0);

		
		double getAngle(double x, double y, double);
		double getRadius();
		double getPercentage();
		double getGenerality();

		void getCentre(double*, double*);
		
		void setRadius(double);
		void setSize(double, double);
		void setCentre(double, double);
		void setLocation(double x, double y);
		void updateMatrix();
		glm::mat4 getMatrix();
		void setGenerality(double);
		void setPercentage(double);
};

#endif
