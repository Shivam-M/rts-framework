#pragma once
#include "../game_new.h"

class Mouse {
	private:
		static Game* game;

	public:
		Mouse(Game*);

		static void scroll_callback(GLFWwindow*, double, double);
		static void callback(GLFWwindow*, int, int, int);
};
