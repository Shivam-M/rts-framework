#include "moveable_new.h"

/*
void MoveableNew::setAlphaShifting(bool state, bool oneway) {
	shiftingAlpha = state;
	oneWay = oneway;
}*/

/*
void MoveableNew::updateColour(short* c, int i, int s) {
	int change = (c[i] - alternate_colour[i]) / 255;
	if (c[i] - alternate_colour[i] < 0) change = 1 ? change == 0 : change;
	else change = -1 ? change == 0 : change;
	c[i] += change * s;
	if (c[i] > 255) c[i] = 255;
	else if (c[i] < 0) c[i] = 0;
}

void MoveableNew::shiftColour(int speed) {
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

void MoveableNew::shiftColour(double speed) {
	Colour2 change = (colour - alternate_colour) * speed;
	colour = colour + (change * speed);
}

/*
void MoveableNew::shiftAlpha(double s) {
	short* new_colour = getColour();
	if (alphaDownwards) {
		if (new_colour[3] <= 0) {
			alphaDownwards = !alphaDownwards; return;
		}
		else new_colour[3] -= max((int)(s * alpha_limit), 1);
	}
	else {
		if (new_colour[3] >= alpha_limit) {
			alphaDownwards = !alphaDownwards; return;
		}
		else new_colour[3] += max((int)(s * alpha_limit), 1);
	}
	memcpy(colour, new_colour, sizeof(colour));
	setColour(colour[0], colour[1], colour[2], colour[3]);
}
*/

/*
float* getDecimals(string a) {
	size_t splitter = a.find(',');
	string first = a.substr(0, splitter);
	string second = a.substr(splitter + 1);
	float values[2] = { stof(first), stof(second) };
	return values;
}*/

void setValues(Vector2& vector, string values) {
	size_t splitter = values.find(',');
	string first = values.substr(0, splitter);
	string second = values.substr(splitter + 1);
	vector.set(stof(first), stof(second));
}

// Add vars.
void MoveableNew::tickTimer(double modifier) {
	if (script_timer > 0) { script_timer -= (1.0 / 60) * modifier; return; }
	if (script_line >= script.size()) return;

	string line = script[script_line];
	size_t splitter = line.find(':');
	string action = line.substr(0, splitter);
	string data = line.substr(splitter + 1);
	for (auto& c : action) c = toupper(c);

	if (action == "WAIT") script_timer = stof(data);
	// else if (action == "LOCATION") { float* values = getDecimals(data); setLocation(values[0], values[1]); }
	else if (action == "LOCATION") { setValues(location, data); }
	// else if (action == "VELOCITY") { float* values = getDecimals(data); setVelocity(values[0], values[1]); }
	else if (action == "VELOCITY") { setValues(velocity, data); }
	// else if (action == "ACCELERATION") { float* values = getDecimals(data); setAcceleration(values[0], values[1]); }
	else if (action == "ACCELERATION") { setValues(acceleration, data); }
	else if (action == "X") location.x = stof(data);
	else if (action == "Y") location.y = stof(data);
	else if (action == "WIDTH") size.x = stof(data);
	else if (action == "HEIGHT") size.y = stof(data);
	// else if (action == "COLOUR") setColour(Colour2::HexToRGB(data));
	else if (action == "ALPHA") colour.setW(stof(data));
	// else if (action == "GRADIENT") setGradientColour(Colour2::HexToRGB(data));
	else if (action == "GRADIENT_ALPHA") gradient_colour.setW(stof(data));
	else if (action == "JUMP") script_line = stoi(data);
	else if (action == "TEXTURE") setTexture(Image::getImage(data));
	script_line++;
}

void MoveableNew::loadScript(string path) {
	stringstream ss;
	info(ss << "Loading script... " << path << " [" << getName() << "]");
	string line;
	fstream in(path);
	while (getline(in, line)) if (line.size() > 0) script.push_back(line);
}

void MoveableNew::common(double modifier) {
	if (shifting_colour) shiftColour(1 * modifier);
	// if (shiftingAlpha) shiftAlpha(0.01 * modifier);
	tickTimer(modifier);
}

void MoveableNew::update(double modifier) {
	common(modifier);

	velocity.x += acceleration.x;
	velocity.y += acceleration.y;

	location.x += velocity.x;
	location.y += velocity.y;

	/*
	if (colour[3] <= 0 && oneWay) {
		setAlphaShifting(false);
		isActive = false;
	}
	*/
}