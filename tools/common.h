#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>

#ifdef _WIN32
    #define FSCANF fscanf_s
    #define SSCANF sscanf_s
    #define SPRINTF(buf, size, format, ...) sprintf_s((buf), (size), (format), __VA_ARGS__)
    #define FOPEN(file, filename, mode) fopen_s(&(file), (filename), (mode))
    #define LOCALTIME(resultptr, timeptr) localtime_s((resultptr), (timeptr))
    #include <format>
#else
    #define FSCANF fscanf
    #define SSCANF sscanf
    #define SPRINTF(buf, size, format, ...) snprintf((buf), (size), (format), __VA_ARGS__)
    #define FOPEN(file, filename, mode) ((file) = fopen((filename), (mode)))
    #define LOCALTIME(resultptr, timeptr) localtime_r((timeptr), (resultptr))
    #include <fmt/core.h>
    namespace std {
        using fmt::format;
    }
#endif

#include <glad/glad.h>
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
    LOCALTIME(&tm, &t);

	cout << put_time(&tm, "\033[1;91m[%d-%m-%Y %H:%M:%S] \033[0m- ");
	log_impl(cout, forward<Args>(args)...);
}

float random_float();

GLuint compile_shader(const char* vertex_shader, const char* fragment_shader);
