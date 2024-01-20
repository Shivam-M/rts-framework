#include "moveable.h"
#include <cmath>


void setValues(Vector2& vector, string values) {
	size_t splitter = values.find(',');
	vector.x = stof(values.substr(0, splitter));
	vector.y = stof(values.substr(splitter + 1));
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

void Moveable::shiftColour() {
	Colour change = (colour_shift.first_colour - colour_shift.second_colour) * colour_shift.speed;
	if (colour_shift.direction == ColourShift::UP) {
		colour = colour + change;
		if (colour >= max(colour_shift.first_colour, colour_shift.second_colour)) {
			colour = max(colour_shift.first_colour, colour_shift.second_colour);
			colour_shift.direction = ColourShift::DOWN;
		}
	}
	else {
		colour = colour - change;
		if (colour <= min(colour_shift.first_colour, colour_shift.second_colour)) {
			colour = min(colour_shift.first_colour, colour_shift.second_colour);
			colour_shift.direction = ColourShift::UP;
		}
	}
	if (colour_shift.with_gradient) gradient_colour = colour;
	if (colour_shift.fade_to_death && colour.getW() <= 0) addFlag(DISABLED);
}

void Moveable::common(float modifier) {
	tickTimer(modifier);
}

void Moveable::update(float modifier) {
	common(modifier);

	velocity.x += acceleration.x;
	velocity.y += acceleration.y;

	location.x += velocity.x;
	location.y += velocity.y;

	if (shifting_colour) {
		shiftColour();
	}
}
