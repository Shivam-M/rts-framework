#pragma once
#include "../game_new.h"

class MouseNew {
	private:
		static GameNew* game;

	public:
		MouseNew(GameNew*);

		static void scroll_callback(GLFWwindow*, double, double);
		static void callback(GLFWwindow*, int, int, int);
};
