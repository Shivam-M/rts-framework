#ifndef COMMON_H
#define COMMON_H
#pragma once

#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>
#include <sstream>

#include <GLFW/glfw3.h>

struct Texture {
	GLuint data;
	int width, height, composition;
	unsigned char* image;
};

enum {
	BACKGROUND	= 1 << 0,
	RING		= 1 << 1,
	CIRCLE		= 1 << 2,
	SQUARE		= 1 << 3,
	FIRE		= 1 << 5,
	STARS		= 1 << 6,
	COLLIDABLE	= 1 << 7,
	PHYSICS		= 1 << 8,
	UNEDITABLE	= 1 << 9,
	TEXT		= 1 << 10,
	ENABLED		= 1 << 11,
	PLAYER		= 1 << 12,
	ENEMY		= 1 << 13,
	MOVING		= 1 << 14,
	BUTTON		= 1 << 15,
	SELECTED	= 1 << 16,
	QUAD		= 1 << 17,
	GHOST		= 1 << 18,
	TEXTURED	= 1 << 19,
	CURVED		= 1 << 20,
	DISABLED	= 1 << 21,
	FIXED_POS	= 1 << 22,
	BOUND		= 1 << 23,
	PROVINCE	= 1 << 24,
	UNIT		= 1 << 25,
};

enum {
	DEFAULT = 0,
	PLANET = 1
};

void info(std::ostream&);
void info(std::string);
void info(int);
void info(double);
void info_w(std::string);
void info_i(std::string);
void info_e(std::string);

short* rgb(std::string);

#endif
