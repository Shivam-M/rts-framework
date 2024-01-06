#ifndef STRUCTS_H
#define STRUCTS_H
#pragma once

#include "../assets/moveable.h"
#include "../assets/text.h"

#include "GLFW/glfw3.h"

using namespace std;


struct Level {
	double player_position[2];
	std::vector<Moveable*> objects;
	std::vector<Text*> text_objects;
};

#endif
