#include "moveable.h"
#include <cmath>

/*
void Moveable::setAlphaShifting(bool state, bool oneway) {
	shiftingAlpha = state;
	oneWay = oneway;
}*/

/*
void Moveable::updateColour(short* c, int i, int s) {
	int change = (c[i] - alternate_colour[i]) / 255;
	if (c[i] - alternate_colour[i] < 0) change = 1 ? change == 0 : change;
	else change = -1 ? change == 0 : change;
	c[i] += change * s;
	if (c[i] > 255) c[i] = 255;
	else if (c[i] < 0) c[i] = 0;
}

void Moveable::shiftColour(int speed) {
	short* new_colour = getColour();
	for (int x = 0; x < 4; x++) updateColour(new_colour, x, speed);
	memcpy(colour, new_colour, sizeof(colour));
	setColour(colour[0], colour[1], colour[2], colour[3]);
	if (colour[0] == alternate_colour[0] &&
		colour[1] == alternate_colour[1] &&
		colour[2] == alternate_colour[2] &&
		colour[3] == alternate_colour[3]) {
		memcpy(alternate_colour, default_colour, sizeof(colour));
	}
}
*/

void Moveable::shiftColour2() {
	Colour change = colour_shift.first_colour - colour_shift.second_colour;
	if (colour_shift.direction == ColourShift::UP) {
		colour = colour + (change * colour_shift.speed);
		if (colour >= max(colour_shift.first_colour, colour_shift.second_colour)) {
			colour = max(colour_shift.first_colour, colour_shift.second_colour);
			colour_shift.direction = ColourShift::DOWN;
		}
	} else {
		colour = colour - (change * colour_shift.speed);
		if (colour <= min(colour_shift.first_colour, colour_shift.second_colour)) {
			colour = min(colour_shift.first_colour, colour_shift.second_colour);
			colour_shift.direction = ColourShift::UP;
		}
	}
}

void Moveable::shiftColour(float speed) {
	if (getFlags() & TEXT) {
		return;
	}
	Colour change = (alternate_colour - colour) * speed;
	colour = colour + (change * speed);
}

void setValues(Vector2& vector, string values) {
	size_t splitter = values.find(',');
	string first = values.substr(0, splitter);
	string second = values.substr(splitter + 1);
	vector.set(stof(first), stof(second));
}

// Add vars.
void Moveable::tickTimer(float modifier) {
	if (script_timer > 0) { script_timer -= (1.0 / 60) * modifier; return; }
	if (script_line >= script.size()) return;

	string line = script[script_line];
	size_t splitter = line.find(':');
	string action = line.substr(0, splitter);
	string data = line.substr(splitter + 1);
	for (auto& c : action) c = toupper(c);

	if (action == "WAIT") script_timer = stof(data);
	else if (action == "LOCATION") { setValues(location, data); }
	else if (action == "VELOCITY") { setValues(velocity, data); }
	else if (action == "ACCELERATION") { setValues(acceleration, data); }
	else if (action == "X") location.x = stof(data);
	else if (action == "Y") location.y = stof(data);
	else if (action == "WIDTH") size.x = stof(data);
	else if (action == "HEIGHT") size.y = stof(data);
	else if (action == "COLOUR") setColour(Colour::HexToRGB(data));
	else if (action == "ALPHA") colour.setW(stof(data));
	else if (action == "GRADIENT") setGradientColour(Colour::HexToRGB(data));
	else if (action == "GRADIENT_ALPHA") gradient_colour.setW(stof(data));
	else if (action == "JUMP") script_line = stoi(data);
	else if (action == "TEXTURE") setTexture(Image::getImage(data));
	script_line++;
}

void Moveable::loadScript(string path) {
	log_t("Loading script... " + path + " [" + getName() + "]");
	string line;
	fstream in(path);
	while (getline(in, line)) if (line.size() > 0) script.push_back(line);
}

void Moveable::common(float modifier) {
	if (shifting_colour) shiftColour(1 * modifier);
	tickTimer(modifier);
}

void Moveable::update(float modifier) {

	common(modifier);

	velocity.x += acceleration.x;
	velocity.y += acceleration.y;

	location.x += velocity.x;
	location.y += velocity.y;

	if (getFlags() & PROVINCE) {
		shiftColour2();
	}

	alternate_colour = Colour(0, 0, 0, 0);
	// shiftColour(0.125f);

	/*
	if (colour[3] <= 0 && oneWay) {
		setAlphaShifting(false);
		isActive = false;
	}
	*/
}
