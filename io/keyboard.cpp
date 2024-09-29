#include "keyboard.h"

Game* Keyboard::game = nullptr;
Keyboard::Keyboard(Game* instance) { Keyboard::game = instance; }

void Keyboard::movement(int direction, int action) {
	switch (direction) {
		case GLFW_KEY_LEFT:
			if (action == GLFW_PRESS) game->player.setAcceleration(-0.0001, game->player.acceleration.y);
			if (action == GLFW_RELEASE) game->player.stop();
			break;
		case GLFW_KEY_RIGHT:
			if (action == GLFW_PRESS) game->player.setAcceleration(0.0001, game->player.acceleration.y);
			if (action == GLFW_RELEASE) game->player.stop();
			break;
		case GLFW_KEY_UP:
			if (action == GLFW_PRESS) game->player.executeJump();
			break;
	}
}

void Keyboard::callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key >= GLFW_KEY_RIGHT and key <= GLFW_KEY_UP)			movement(key, action);

	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_F && !game->console->visible())		game->render.toggleFullscreen();
		else if (key == GLFW_KEY_TAB)							game->debugMode();
		else if (key == GLFW_KEY_GRAVE_ACCENT)					game->console->toggle();
		else if (key == GLFW_KEY_ENTER)							game->console->execute();
		else if (key == GLFW_KEY_Z)								game->toggleDebug();
	} else {
		if (game->console->visible())
			if ((key >= 65 && key <= 90 || key == 259 || key == 32 || (key >= 45 && key <= 57))) game->console->entry(key);
	}
}
