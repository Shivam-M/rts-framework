#pragma once

#include <map>
#include <string>
#include <json.hpp>

using namespace std;

struct Texture {
	~Texture() { if (image) delete[] image; }
	unsigned int data = 0;
	int width = 0, height = 0, composition = 0;
	unsigned char* image = nullptr;
	string path;
};

class Image {
	public:
		static map<string, Texture*> images;

		static void cleanup();
		static Texture* get_image(const string&);
		static void load_image(const string&);
		static void load_map(const string&, const string&);
};
