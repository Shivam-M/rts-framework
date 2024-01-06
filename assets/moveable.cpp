#include "moveable.h"


vector<map<double, double>> Moveable::getPoints() { return vector<map<double, double>>(); }
Text*   Moveable::getText() { return text; }
Texture* Moveable::getTexture() { return texture; };
string  Moveable::getName() { return name; }
double* Moveable::getBounds() { return bounds; }
double* Moveable::getGLGradient() { return gl_gradient[0] < 0 ? nullptr : gl_gradient; }
double* Moveable::getAcceleration() { return acceleration; }
double* Moveable::getVelocity() { return velocity; }
double* Moveable::getLocation() { return location; }
double* Moveable::getSize() { return size; }
double* Moveable::getGLColour() { return gl_colour; }
short*	Moveable::getColour() { return colour; }
int		Moveable::getFlags() { return flags; }
void	Moveable::addFlag(int f) { flags |= f; }
void	Moveable::removeFlag(int f) { flags &= ~f; }

void Moveable::setText(Text* t) { text = t; }
void Moveable::setTexture(Texture* t) { addFlag(TEXTURED); texture = t; };
void Moveable::setName(string n) { name = n; }
void Moveable::setBounds(double x, double y) { bounds[0] = x, bounds[1] = y; }
void Moveable::setAcceleration(double x, double y) { acceleration[0] = x, acceleration[1] = y; }
void Moveable::setVelocity(double x, double y) { velocity[0] = x, velocity[1] = y; }
void Moveable::setLocation(double x, double y) { location[0] = x, location[1] = y; }
void Moveable::setSize(double width, double height) { size[0] = width, size[1] = height; }
void Moveable::setDefaultColour(short r, short g, short b, short a) { default_colour[0] = r, default_colour[1] = g, default_colour[2] = b, default_colour[3] = a; }
void Moveable::setAlternateColour(short r, short g, short b, short a) { alternate_colour[0] = r, alternate_colour[1] = g, alternate_colour[2] = b, alternate_colour[3] = a; }


void normaliseCoordinates2(double* x, double* y) {
	*x = *x < 0.5 ? -1 + (*x * 2) : *x > 0.5 ? ((*x - 0.5) * 2) : 0;
	*y = *y < 0.5 ? (-1 + (*y * 2)) * -1 : *y > 0.5 ? ((*y - 0.5) * 2) * -1 : 0;
}

void Moveable::resetColour() { memcpy(&colour, &default_colour, sizeof(colour)); }
void Moveable::updateMatrix2() {
	double new_x = location[0]; double new_y = location[1];
	normaliseCoordinates2(&new_x, &new_y);
	new_x += size[0], new_y -= size[1];
	matrix2 = glm::mat4(1.f);
	matrix2 = glm::translate(matrix2, glm::vec3(new_x, new_y, 0.0f));
	matrix2 = glm::scale(matrix2, glm::vec3(size[0], size[1], 0.0f));
}

glm::mat4 Moveable::getMatrix2() {
	return matrix2;
}

void Moveable::setColour(short* colour, short alpha) { setColour(colour[0], colour[1], colour[2], alpha); }
void Moveable::setGradientColour(short* colour, short alpha) { setGradientColour(colour[0], colour[1], colour[2], alpha); }
void Moveable::setColourShifting(bool state) { shiftingColour = state; }

void Moveable::setAlphaShifting(bool state, bool oneway) { 
	shiftingAlpha = state;
	oneWay = oneway;
}

void Moveable::setColour(short r, short g, short b, short a) {
	colour[0] = r, colour[1] = g, colour[2] = b, colour[3] = a;
	gl_colour[0] = r / 255.0, gl_colour[1] = g / 255.0, gl_colour[2] = b / 255.0, gl_colour[3] = a / 255.0;
}

void Moveable::setGradientColour(short r, short g, short b, short a) {
	gradient_colour[0] = r, gradient_colour[1] = g, gradient_colour[2] = b, gradient_colour[3] = a;
	gl_gradient[0] = r / 255.0, gl_gradient[1] = g / 255.0, gl_gradient[2] = b / 255.0, gl_gradient[3] = a / 255.0;
}

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

void Moveable::shiftAlpha(double s) {
	short* new_colour = getColour();
	if (alphaDownwards) {
		if (new_colour[3] <= 0) {
			alphaDownwards = !alphaDownwards; return;
		}
		else new_colour[3] -= max((int)(s * alpha_limit), 1);
	} else {
		if (new_colour[3] >= alpha_limit) {
			alphaDownwards = !alphaDownwards; return;
		}
		else new_colour[3] += max((int)(s * alpha_limit), 1);
	}
	memcpy(colour, new_colour, sizeof(colour));
	setColour(colour[0], colour[1], colour[2], colour[3]);
}

float* getDecimals(string a) {
	size_t splitter = a.find(',');
	string first = a.substr(0, splitter);
	string second = a.substr(splitter + 1);
	float values[2] = { stof(first), stof(second) };
	return values;
}

// Add vars.
void Moveable::tickTimer(double modifier) {
	if (timer > 0) { timer -= (1.0 / 60) * modifier; return; }
	if (script_line >= script.size()) return;

	string line = script[script_line];
	size_t splitter = line.find(':');
	string action = line.substr(0, splitter);
	string data = line.substr(splitter + 1);
	for (auto& c : action) c = toupper(c);

	if (action == "WAIT") timer = stof(data);
	else if (action == "LOCATION") { float* values = getDecimals(data); setLocation(values[0], values[1]); }
	else if (action == "VELOCITY") { float* values = getDecimals(data); setVelocity(values[0], values[1]); }
	else if (action == "ACCELERATION") { float* values = getDecimals(data); setAcceleration(values[0], values[1]); }
	else if (action == "X") setLocation(stof(data), location[1]);
	else if (action == "Y") setLocation(location[0], stof(data));
	else if (action == "WIDTH") setSize(stof(data), size[1]);
	else if (action == "HEIGHT") setSize(size[0], stof(data));
	else if (action == "COLOUR") setColour(rgb(data));
	else if (action == "ALPHA") setColour(colour[0], colour[1], colour[2], stof(data));
	else if (action == "GRADIENT") setGradientColour(rgb(data));
	else if (action == "GRADIENT_ALPHA") setGradientColour(gradient_colour[0], gradient_colour[1], gradient_colour[2], stof(data));
	else if (action == "JUMP") script_line = stoi(data);
	else if (action == "TEXTURE") setTexture(Image::getImage(data));
	script_line++;
}

void Moveable::loadScript(string path) {
	stringstream ss;
	info(ss << "Loading script... " << path << " [" << getName() << "]");
	string line;
	fstream in(path);
	while (getline(in, line)) if (line.size() > 0) script.push_back(line);
	// info("script size " + to_string(script.size()));
	// addFlag(MOVING);
}

void Moveable::common(double modifier) {
	if (shiftingColour) shiftColour(1 * modifier);
	if (shiftingAlpha) shiftAlpha(0.01 * modifier);
	tickTimer(modifier);
}

void Moveable::update(double modifier) {
	common(modifier);
	velocity[0] += acceleration[0];
	velocity[1] += acceleration[1];
	location[0] += velocity[0];
	location[1] += velocity[1];

	if (colour[3] <= 0 && oneWay) {
		setAlphaShifting(false);
		isActive = false;
	}
}
