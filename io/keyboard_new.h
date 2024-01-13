#pragma once
#include "../game_new.h"

class KeyboardNew {
	private:
		static GameNew* game;

	public:
		KeyboardNew(GameNew*);

		static void movement(int direction, int action = -1);
		static void callback(GLFWwindow*, int, int, int, int);
};
