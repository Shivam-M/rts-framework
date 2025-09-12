#include "keyboard.h"

#include "../game.h"
#include "../assets/player.h"
#include "../tools/common.h"
#include "../tools/render.h"
#include "../tools/console.h"
#include "../io/mouse.h"

Game* Keyboard::game = nullptr;
Keyboard::Keyboard(Game* instance) { Keyboard::game = instance; }

void Keyboard::movement(int direction, int action) {
	switch (direction) {
		case GLFW_KEY_LEFT:
			if (action == GLFW_PRESS) game->player->set_acceleration(-0.0001, game->player->acceleration.y);
			if (action == GLFW_RELEASE) game->player->stop();
			break;
		case GLFW_KEY_RIGHT:
			if (action == GLFW_PRESS) game->player->set_acceleration(0.0001, game->player->acceleration.y);
			if (action == GLFW_RELEASE) game->player->stop();
			break;
		case GLFW_KEY_UP:
			if (action == GLFW_PRESS) game->player->jump();
			break;
	}
}
void Keyboard::callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key >= GLFW_KEY_RIGHT and key <= GLFW_KEY_UP)			movement(key, action);

	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_F && !game->console->visible())		game->render->toggle_fullscreen();
		else if (key == GLFW_KEY_TAB)							game->debug_mode();
		else if (key == GLFW_KEY_GRAVE_ACCENT)					game->console->toggle();
		else if (key == GLFW_KEY_ENTER)							game->console->execute();
		else if (key == GLFW_KEY_Z)								game->toggle_debug();
	} else {
		if (game->console->visible()) {
			if ((key == GLFW_KEY_MINUS) && (mods & GLFW_MOD_SHIFT)) game->console->entry('_');
			else game->console->entry(key);
		}
		else if (game->mouse->debug_control_scheme) {
			// game->debug_callback(key, action);
		}
	}
}
