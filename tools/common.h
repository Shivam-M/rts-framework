#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <format>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define CON_RED		"\033[1;91m"
#define CON_YELLOW	"\033[1;33m"
#define CON_CYAN	"\033[1;32m"
#define CON_LBLUE	"\033[1;36m"
#define CON_NORMAL  "\033[0m"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

using namespace std;

struct Texture {
	GLuint data = 0;
	int width = 0, height = 0, composition = 0;
	unsigned char* image = nullptr;
	string path;
};

enum {
    BACKGROUND =        1 << 0,
    COLLIDABLE =        1 << 1,
    UNEDITABLE =        1 << 2,
    TEXT =              1 << 3,
    ENABLED =           1 << 4,
    PLAYER =            1 << 5,
    MOVING =            1 << 6,
    BUTTON =            1 << 7,
    SELECTED =          1 << 8,
    QUAD =              1 << 9,
    TEXTURED =          1 << 10,
    CURVED =            1 << 11,
    DISABLED =          1 << 12,
    FIXED_POS =         1 << 13,
    BOUND =             1 << 14,
    PROVINCE =          1 << 15,
    UNIT =              1 << 16,
    NATION =            1 << 17,
    CUSTOM =            1 << 18,
    PANEL =             1 << 19,
    PARTICLES =         1 << 20,
    TEXT_BACKGROUND =   1 << 21,
    THEMED =            1 << 22,
    DEBUG =             1 << 23,
    UNSAVEABLE =        1 << 24,
    UI =                1 << 25,
    UNDER_PANEL =       1 << 26,
    DRAGGABLE =         1 << 27,
    NO_RENDER =         1 << 28
};
 
void log_impl(ostream& os);

template <typename First, typename... Rest>
void log_impl(ostream& os, First&& first, Rest&&... rest) {
	ostringstream ss;
	ss << forward<First>(first);
	os << ss.str();
	log_impl(os, forward<Rest>(rest)...);
}

template <typename... Args>
void log_t(Args&&... args) {
	auto t = time(nullptr);
	tm tm;
	localtime_s(&tm, &t);

	cout << put_time(&tm, "\033[1;91m[%d-%m-%Y %H:%M:%S] \033[0m- ");
	log_impl(cout, forward<Args>(args)...);
}

float random_float();

GLuint CompileShader(const char* vertex_shader, const char* fragment_shader);
