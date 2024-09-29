#pragma once
#include "../game.h"

class Mouse {
	private:
		static Game* game;

	public:
		Mouse(Game*);

		static inline bool debug_control_scheme = false;

		static void scroll_callback(GLFWwindow*, double, double);
		static void callback(GLFWwindow*, int, int, int);
};
