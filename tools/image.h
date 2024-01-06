#ifndef IMAGE_H
#define IMAGE_H
#pragma once

#include <iostream>
#include <map>
#include <GLFW/glfw3.h>
#include <json.hpp>
#include "common.h"

using namespace std;
using namespace nlohmann;

class Image {
	public:
		static map<string, Texture*> images;

		static void loadImage(string);
		static Texture* getImage(string);

		static void loadMap(string, string);

};
#endif