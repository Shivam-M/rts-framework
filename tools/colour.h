#ifndef COLOUR_H
#define COLOUR_H
#pragma once

#define WHITE			Colour(255, 255, 255)
#define BLACK			Colour(0, 0, 0)
#define DARK_GREY		Colour("222F3E")
#define LIGHT_GREY		Colour("ECF0F1")
#define TRANSPARENT		Colour(0, 0, 0, 0);

#include <iostream>
#include <algorithm>
#include <iterator>

using namespace std;

class Colour {
	private:
		float rgb_[4]{ 1, 1, 1, 1 };

	public:
		Colour(string, float = 1.0);
		// Colour(float, float, float, float = 1.0);
		Colour(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255); // : Colour(r / 255., g / 255., b / 255., a / 255.) {}

		void setAlpha(float);
		void increaseAlpha(float);
		
		bool equals(Colour);

		float getAlpha();
		float* getRGB();
		static float* hexToRGB(float*, string, float = 1.0);
};

#endif