#pragma once

#include <map>
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
