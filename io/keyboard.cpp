
#include "keyboard.h"

Game* Keyboard::game = nullptr;
Keyboard::Keyboard() {}

Keyboard::Keyboard(Game* g) {
	this->game = g;
}

void Keyboard::movement(int direction, int action) {
	switch (direction) {
		case game->LEFT:
			if (game->player.mode == DEFAULT) {
				if (action == game->PRESS) game->player.setAcceleration(-0.0001, game->player.getAcceleration()[1]);
				if (action == game->RELEASE) game->player.stop();
			} else {
				if (action == game->PRESS) game->player.increaseRotation(-5);
				if (action == game->RELEASE) game->player.stopRotation();
			}
			return;
		case game->RIGHT:
			if (game->player.mode == DEFAULT) {
				if (action == game->PRESS) game->player.setAcceleration(0.0001, game->player.getAcceleration()[1]);
				if (action == game->RELEASE) game->player.stop();
			} else {
				if (action == game->PRESS) game->player.increaseRotation(5);
				if (action == game->RELEASE) game->player.stopRotation();
			}
			return;
		case game->UP:
			if (game->player.mode == DEFAULT) {
				if (action == game->PRESS) game->player.executeJump();
			} else {
				if (action == game->PRESS) game->player.executeJump(); // temp
			}
			return;
		case game->DOWN: return;
	}
}


void Keyboard::callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// printf("%d\n", key);
	if (action == GLFW_RELEASE) {
		// if (key == 70) game->render.toggleFullscreen();
		if (key == 262) movement(game->RIGHT, game->RELEASE);
		if (key == 263) movement(game->LEFT, game->RELEASE);
		if (key == 258) game->debugMode();
		if (key == 96) game->console->toggle();
	}
	else {
		if (key == 262) movement(game->RIGHT);
		if (key == 263) movement(game->LEFT);
		if (key == 265) movement(game->UP);
		if (key == 257) game->console->execute();
		if ((key >= 65 && key <= 90 || key == 259 || key == 32 || (key >= 45 && key <= 57)) && game->console->visible()) game->console->entry(key);
		else if (key == 90) game->fireShot();
	}
}
