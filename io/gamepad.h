#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../game.h"

class Gamepad {
private:
	static Game* game;
	double deadzone[2] = { 0.15, 0.15 };

public:
	Gamepad();
	Gamepad(Game*);

	void update();
};