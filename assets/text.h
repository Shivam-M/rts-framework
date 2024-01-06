#ifndef TEXT_H
#define TEXT_H
#pragma once
// #include <iostream>
#include "../tools/TextRenderer.hpp"
#include "../tools/common.h"
#include "moveable.h"

using namespace std;
using namespace glfreetype;

class Text {
	private:
		int flags = ENABLED;
		string content;
		double location[2] = { 0.0, 0.0 };
		double size[2] = { 0.075, 0.02 };
		Moveable* collision_box = nullptr;

		font_data font;
		int colour[4] = { 255, 255, 255, 255 };
		double gl_colour[4] = { 1.0, 1.0, 1.0, 1.0 };

	public:
		Text();
		Text(double, double, font_data, int, int, int, int, string = "");

		double* getSize();
		double* getLocation();
		double* getGLColour();
		int* getColour();
		int getFlags();

		font_data getFont();
		string getContent();
		Moveable* getCollisionBox();

		void setLocation(double, double);
		void setFont(font_data);
		void setColour(int, int, int, int = 255);
		void setContent(string);
		void setSize(double, double);
		void setCollisionBox(Moveable*);
		void calculateBounds();

		void addFlag(int);
		void removeFlag(int);

		bool fixed = true;
};

#endif
