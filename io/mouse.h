#pragma once
#include "../game.h"

class Mouse {
	private:
		static Game* game;

	public:
		static inline bool debug_control_scheme = true;

		Mouse(Game*);

		static void scroll_callback(GLFWwindow*, double, double);
		static void callback(GLFWwindow*, int, int, int);
};
