#include "gamepad.h"

Game* Gamepad::game = nullptr;
Gamepad::Gamepad() {}

Gamepad::Gamepad(Game* g) {
	this->game = g;
}

void Gamepad::update() {
	/*
	const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);

	if (name == NULL) return;

	GLFWgamepadstate state;
	glfwGetGamepadState(GLFW_JOYSTICK_1, &state);

	double x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
	double y = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

	if (x >= deadzone[0]) {
		Keyboard::movement(game->RIGHT, game->PRESS);
		game->getPlayer()->max_speed = 0.00375 * abs(x);
	} else if (x <= -deadzone[0]) {
		Keyboard::movement(game->LEFT, game->PRESS);
		game->getPlayer()->max_speed = 0.00375 * abs(x);
	} else {
		Keyboard::movement(game->RIGHT, game->RELEASE);
		game->getPlayer()->max_speed = 0.00375;
	}*/
}