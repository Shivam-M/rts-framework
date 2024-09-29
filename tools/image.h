#pragma once

#include <map>
#include <json.hpp>
#include "common.h"

using namespace std;
using namespace nlohmann;

class Image {
	public:
		static map<string, Texture*> images;

		static Texture* getImage(string);
		static void loadImage(string);
		static void loadMap(string, string);
};
