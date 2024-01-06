#include "colour.h"

using namespace std;

Colour::Colour(string h, float alpha) {
	hexToRGB(rgb_, h, alpha);
}

Colour::Colour(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	rgb_[0] = r / 255.0; rgb_[1] = g / 255.0; rgb_[2] = b / 255.0; rgb_[3] = a / 255.0;
}

/*
Colour::Colour(double r, double g, double b, double a) {
	rgb_[0] = r; rgb_[1] = g; rgb_[2] = b; rgb_[3] = a;
}
*/

float Colour::getAlpha() {
	return rgb_[3];
}

float* Colour::getRGB() {
	return rgb_;
}

float* Colour::hexToRGB(float* rgb, string hex, float alpha) {
	float rgba[4]{ 0, 0, 0, alpha };
	sscanf_s(hex.c_str(), "%02x%02x%02x", &rgb[0], &rgb[1], &rgb[1]);
	for (int i = 0; i < 3; i++) rgba[i] = rgb[i] / 255.0;
	return rgba;
}

void Colour::setAlpha(float alpha) {
	rgb_[3] = alpha;
}


void Colour::increaseAlpha(float value) {
	rgb_[3] += value;
}

bool Colour::equals(Colour c) {
	return (equal(rgb_, rgb_ + sizeof rgb_ / sizeof * rgb_, c.getRGB()));
}
