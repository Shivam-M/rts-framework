#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../game.h"

class Mouse {
private:
	static Game* game;

public:
	Mouse();
	Mouse(Game*);

	static void scroll_callback(GLFWwindow*, double, double);
	static void callback(GLFWwindow*, int, int, int);
};