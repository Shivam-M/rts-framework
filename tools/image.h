#pragma once

#include <map>
#include <json.hpp>

using namespace std;

struct Texture;

class Image {
	public:
		static map<string, Texture*> images;

		static Texture* get_image(const string&);
		static void load_image(const string&);
		static void load_map(const string&, const string&);
};
