#pragma once

#include <map>
#include <json.hpp>

using namespace std;

struct Texture;

class Image {
	public:
		static map<string, Texture*> images;

		static Texture* getImage(const string&);
		static void loadImage(const string&);
		static void loadMap(const string&, const string&);
};
