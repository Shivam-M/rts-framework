#include "moveable.h"

#include "../assets/text.h"
#include "../tools/image.h"
#include "../tools/common.h"

#include <cmath>
#include <fstream>
#include <algorithm>

using namespace std;

void Moveable::set_text_offset(float x, float y) {
	text_offset.set(x, y); 
	if (text) text->set_location(location.x + text_offset.x, location.y + text_offset.y);
}

void setValues(Vector2& vector, string values) {
	size_t splitter = values.find(',');
	vector.x = stof(values.substr(0, splitter));
	vector.y = stof(values.substr(splitter + 1));
}

// Add vars.
void Moveable::tick_timer(const float& modifier) {
	if (script == nullptr) return;
	if (script_timer > 0) { script_timer -= (1.0f / 60) * modifier; return; }
	if (script_line >= script->size()) return;

	vector<string> dereferenced_sript = *script;  // todo

	string line = dereferenced_sript[script_line];
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
	else if (action == "COLOUR") set_colour(Colour::hex_to_rgb(data));
	else if (action == "ALPHA") colour.set_alpha(stof(data));
	else if (action == "GRADIENT") set_gradient_colour(Colour::hex_to_rgb(data));
	else if (action == "GRADIENT_ALPHA") gradient_colour.set_alpha(stof(data));
	else if (action == "JUMP") script_line = stoi(data);
	else if (action == "TEXTURE") set_texture(Image::get_image(data));
	// TODO else if (action == "COLOUR_SHIFT");
	script_line++;
}

void Moveable::load_script(string path) {
	log_t("Loading script... " + path + " [" + get_name() + "]");
	string line;
	fstream in(path);
	while (getline(in, line))
		if (line.size() > 0)
			if (script) {
				script->push_back(line);
			} 
			else {
				script = new vector<string>();
				script->push_back(line);
			}
}

void Moveable::shift_colour() {
	colour_shift.progress += (colour_shift.direction == ColourShift::Direction::Up ? colour_shift.speed : -colour_shift.speed);

	if (colour_shift.progress >= 1.0f || colour_shift.progress <= 0.0f) {
		colour_shift.progress = clamp(colour_shift.progress, 0.0f, 1.0f);
		if (colour_shift.loop) {
			colour_shift.direction = (colour_shift.progress == 1.0f ? ColourShift::Direction::Down : ColourShift::Direction::Up);
		} else {
			stop_colour_shift();
		}
	}

	if (colour_shift.with_gradient) gradient_colour = colour;
	if (colour_shift.fade_to_death && colour.get_w() <= 0)  add_flag(DISABLED);
	if (colour_shift.condition && *colour_shift.condition != colour_shift.target) stop_colour_shift();
}

void Moveable::stop_colour_shift() {
	shifting_colour = false;
	// if (filters.size() > 0 && filters.front()->mode == ColourFilter::Mode::Replacement) filters.pop_front();
}

void Moveable::common(const float& modifier) {
	tick_timer(modifier);
}

void Moveable::update(const float& modifier) {
	common(modifier);

	velocity.x += acceleration.x;
	velocity.y += acceleration.y;

	location.x += velocity.x;
	location.y += velocity.y;

	evaluated_colour = colour;

	if (shifting_colour) {
		shift_colour();
		evaluated_colour = colour_shift.evaluate();
	}

	for (ColourFilter* filter : filters) {
		switch (filter->mode) {
			case ColourFilter::Mode::Replacement:
				evaluated_colour = filter->colour;
				break;
			case ColourFilter::Mode::Addition:
				evaluated_colour += filter->colour;
				break;
			case ColourFilter::Mode::Multiplication:
				evaluated_colour *= filter->colour;
				break;
		}
	}
}

void Moveable::on_hover() {
	if (!shifting_colour) colour.w_ /= 1.25;
	if (hover_tooltip && has_flag(BUTTON)) {
		get_text()->remove_flag(DISABLED);
	}
}

void Moveable::on_hover_stop() {
	if (!shifting_colour) reset_colour();
	if (hover_tooltip && has_flag(BUTTON)) {
		get_text()->add_flag(DISABLED);
	}
}
