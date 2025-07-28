#pragma once

#include <map>
#include <json.hpp>
#include "common.h"

using namespace std;
using namespace nlohmann;

class Image {
	public:
		static map<string, Texture*> images;

		static Texture* getImage(const string&);
		static void loadImage(const string&);
		static void loadMap(const string&, const string&);
};
