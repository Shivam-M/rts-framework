#include "keyboard_new.h"

GameNew* KeyboardNew::game = nullptr;
KeyboardNew::KeyboardNew(GameNew* instance) { KeyboardNew::game = instance; }

void KeyboardNew::movement(int direction, int action) {
	switch (direction) {
		case GLFW_KEY_LEFT:
			return;
		case GLFW_KEY_RIGHT:
			return;
		case GLFW_KEY_UP:
			return;
	}
}

void KeyboardNew::callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (GLFW_KEY_RIGHT >= key && key <= GLFW_KEY_UP)			movement(key, action);

	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_F && !game->console->visible())		game->render.toggleFullscreen();
		else if (key == GLFW_KEY_TAB)							game->debugMode();
		else if (key == GLFW_KEY_GRAVE_ACCENT)					game->console->toggle();
		else if (key == GLFW_KEY_ENTER)							game->console->execute();
	} else {
		if (game->console->visible())
			if ((key >= 65 && key <= 90 || key == 259 || key == 32 || (key >= 45 && key <= 57))) game->console->entry(key);
	}
}
