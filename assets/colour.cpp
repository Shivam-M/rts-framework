#include "colour.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

#ifdef _WIN32
    #define SSCANF sscanf_s
#else
    #define SSCANF sscanf
#endif

Colour Colour::from_hex(const string& hex, float alpha) { 
	int r, g, b;
	SSCANF(hex.c_str(), "%02x%02x%02x", &r, &g, &b);
	return Colour(r, g, b, alpha * 255); 
}

string Colour::to_hex(const Colour& rgb) {
	int r = clamp(static_cast<int>(rgb.x), 0, 255);
	int g = clamp(static_cast<int>(rgb.y), 0, 255);
	int b = clamp(static_cast<int>(rgb.z), 0, 255);

	stringstream ss;
	ss << hex << setfill('0') << setw(2) << r << setw(2) << g << setw(2) << b;
	return ss.str();
}