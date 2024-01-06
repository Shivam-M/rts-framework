#include "circle.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const auto PI = acos(-1);


Circle::Circle() {
	name = "Circle";
	type = 1;
	addFlag(CIRCLE);
}

Circle::Circle(double x, double y, double radius, int r, int g, int b, int a) {
	name = "Circle";
	type = 1;
	addFlag(CIRCLE);
	setLocation(x, y);
	setRadius(radius);
	setColour(r, g, b, a);
}

void Circle::setRadius(double r) { 
	radius_ = r;
	// size[0] = r, size[1] = r * 2;
	// setSize(r, r * 2);
}

double Circle::getAngle(double x, double y, double offset) {
	double sine = (x - centre[0]) / (getRadius() + offset);
	double angle = acos(sine) / (PI / 180);
	if (y < centre[1]) angle = 180 + (180 - angle);
	return 360 - angle;
}

void Circle::setPercentage(double p) { percentage_ = p; }

double Circle::getRadius() { return radius_; }

double Circle::getPercentage() { return percentage_; }

void Circle::setSize(double width, double height) {
	size[0] = width, size[1] = height;
	radius_ = sqrt(pow(width, 2) + pow(height, 2)) / 2;
}

void Circle::setCentre(double x, double y) {
	centre[0] = x; centre[1] = y;
	location[0] = x; location[1] = y;
	// setLocation(x, y);
}

void Circle::getCentre(double* x, double* y) {
	*x = centre[0];
	*y = centre[1];
	// memcpy(c, &centre, sizeof(double) * 2
}

void normaliseCoordinates(double* x, double* y) {
	*x = *x < 0.5 ? -1 + (*x * 2) : *x > 0.5 ? ((*x - 0.5) * 2) : 0;
	*y = *y < 0.5 ? (-1 + (*y * 2)) * -1 : *y > 0.5 ? ((*y - 0.5) * 2) * -1 : 0;
}

void Circle::setLocation(double x, double y) {
	setCentre(x, y);
}

// TODO: Call when window size has changed as scale is fixed
void Circle::updateMatrix() {
	double new_x = location[0]; double new_y = location[1];
	normaliseCoordinates(&new_x, &new_y);
	matrix = glm::mat4(1.f);
	matrix = glm::translate(matrix, glm::vec3(new_x, new_y, 0.0f));
	matrix = glm::scale(matrix, glm::vec3((radius_ * 50) / 16, (radius_ * 50) / 9, 0.0f));
}

glm::mat4 Circle::getMatrix() {
	return matrix;
}

void Circle::update(double modifier) {
	common(modifier);

	if (colour[3] <= 0 && oneWay) {
		setAlphaShifting(false);
		isActive = false;
		
		// printf("No longer active...\n");
		// free and cleanup later...
	}
}

void Circle::setGenerality(double value) {
	generality_ = value;
}

double Circle::getGenerality() {
	return generality_;
}