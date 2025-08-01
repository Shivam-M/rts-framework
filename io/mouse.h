#pragma once

class Game;
struct GLFWwindow;

class Mouse {
	private:
		static Game* game;

	public:
		Mouse(Game*);

		static inline bool debug_control_scheme = false;

		static void scroll_callback(GLFWwindow* window, double x, double y);
		static void callback(GLFWwindow* window, int button, int action, int mods);
};
