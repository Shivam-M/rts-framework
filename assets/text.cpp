#include "Text.h"

#include <algorithm>

double* Text::getSize() { return size; }
double* Text::getLocation() { return location; }
double* Text::getGLColour() { return gl_colour; }
int*	Text::getColour() { return colour; }
int		Text::getFlags() { return flags; }
void	Text::addFlag(int f) { flags |= f; }
void	Text::removeFlag(int f) { flags &= ~f; }

font_data	Text::getFont() { return font; }
string		Text::getContent() { return content; }
Moveable*   Text::getCollisionBox() { return collision_box; }

void Text::setCollisionBox(Moveable* b) { collision_box = b; }
void Text::setFont(font_data f) { font = f; }
void Text::setContent(string c) { content = c; calculateBounds(); }
void Text::setSize(double width, double height) { size[0] = width, size[1] = height; }
void Text::setLocation(double x, double y) { location[0] = x, location[1] = y; }

void Text::setColour(int r, int g, int b, int a) {
	colour[0] = r, colour[1] = g, colour[2] = b, colour[3] = a;
	gl_colour[0] = r / 255.0, gl_colour[1] = g / 255.0, gl_colour[2] = b / 255.0, gl_colour[3] = a / 255.0;
}

// temp font-specific solution
void Text::calculateBounds() {
	//if (Moveable* c = getCollisionBox()) delete getCollisionBox();
	Moveable box = Moveable();
	int multiplier = 1;
	int content_limit = content.length();
	int last_nl = 0;
	vector<int> lengths;
	for (size_t i = 0; i < content.length(); i++)
		if (content[i] == '\n') {
			multiplier++;
			if (last_nl == 0) {
				content_limit = i - last_nl;
			}
			else {
				content_limit = max(int(i - last_nl), content_limit);
			}
			last_nl = i;
		}
	// if (multiplier > 1) multiplier++;
	box.setSize(0.0007 * getFont().h * content_limit, (getFont().h * 0.0025) * multiplier);
	box.setLocation(location[0] - 0.005, location[1] - (getFont().h * 0.002));
	box.setColour(236, 240, 241, 40);
	setCollisionBox(&box);
}

Text::Text(double x, double y, glfreetype::font_data f, int r, int g, int b, int a, string c) {
	setLocation(x, y);
	setFont(f);
	setColour(r, g, b, a);
	setContent(c);
}

Text::Text() {}