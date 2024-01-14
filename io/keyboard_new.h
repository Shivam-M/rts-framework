#pragma once
#include "../game_new.h"

class Keyboard {
	private:
		static Game* game;

	public:
		Keyboard(Game*);

		static void movement(int direction, int action = -1);
		static void callback(GLFWwindow*, int, int, int, int);
};
