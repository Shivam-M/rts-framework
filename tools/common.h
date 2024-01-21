#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#include <GLFW/glfw3.h>

#define CON_RED		"\033[1;91m"
#define CON_YELLOW	"\033[1;33m"
#define CON_CYAN	"\033[1;32m"
#define CON_LBLUE	"\033[1;36m"
#define CON_NORMAL  "\033[0m"

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
	FIRE		= 1 << 4,
	STARS		= 1 << 5,
	COLLIDABLE	= 1 << 6,
	UNEDITABLE	= 1 << 7,
	TEXT		= 1 << 8,
	ENABLED		= 1 << 9,
	PLAYER		= 1 << 10,
	ENEMY		= 1 << 11,
	MOVING		= 1 << 12,
	BUTTON		= 1 << 13,
	SELECTED	= 1 << 14,
	QUAD		= 1 << 15,
	TEXTURED	= 1 << 16,
	CURVED		= 1 << 17,
	DISABLED	= 1 << 18,
	FIXED_POS	= 1 << 19,
	BOUND		= 1 << 20,
	PROVINCE	= 1 << 21,
	UNIT		= 1 << 22,
	CUSTOM		= 1 << 23,
	PANEL		= 1 << 24,
	PARTICLES	= 1 << 25
};
 
void log_impl(std::ostream& os);

template <typename First, typename... Rest>
void log_impl(std::ostream& os, First&& first, Rest&&... rest) {
	std::ostringstream ss;
	ss << std::forward<First>(first);
	os << ss.str();
	log_impl(os, std::forward<Rest>(rest)...);
}

template <typename... Args>
void log_t(Args&&... args) {
	auto t = time(nullptr);
	tm tm;
	localtime_s(&tm, &t);

	std::cout << std::put_time(&tm, "\033[1;91m[%d-%m-%Y %H:%M:%S] \033[0m- ");
	log_impl(std::cout, std::forward<Args>(args)...);
}

float random_float();