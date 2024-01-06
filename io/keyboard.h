#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../game.h"

class Keyboard {
	private:
		static Game* game;

	public:
		Keyboard();
		Keyboard(Game*);

		static void movement(int direction, int action = -1);
		static void callback(GLFWwindow*, int, int, int, int);
};