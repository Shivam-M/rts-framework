#include "colour.h"

#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

#ifdef _WIN32
    #define SSCANF sscanf_s
#else
    #define SSCANF sscanf
#endif

template<typename T>
T clamp(T value, T min, T max) {
	return (value < min) ? min : (value > max) ? max : value;
}

Colour Colour::hex_to_rgb(const string& hex, const float& alpha) { 
	int r, g, b;
	SSCANF(hex.c_str(), "%02x%02x%02x", &r, &g, &b);
	return Colour(r, g, b, alpha * 255); 
}

string Colour::rgb_to_hex(Vector4 rgb) {
	int r = clamp(static_cast<int>(rgb.get_x()), 0, 255);
	int g = clamp(static_cast<int>(rgb.get_y()), 0, 255);
	int b = clamp(static_cast<int>(rgb.get_z()), 0, 255);

	stringstream ss;
	ss << hex << setfill('0') << setw(2) << r << setw(2) << g << setw(2) << b;
	return ss.str();
}