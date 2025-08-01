#pragma once

class Game;
struct GLFWwindow;

class Keyboard {
	private:
		static Game* game;

	public:
		Keyboard(Game*);

		static void movement(int direction, int action = -1);
		static void callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
