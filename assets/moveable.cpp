#include "moveable.h"

#include "../assets/text.h"
#include "../tools/image.h"
#include "../tools/common.h"

#include <cmath>
#include <fstream>
#include <algorithm>

using namespace std;

void Moveable::set_text_offset(float x, float y) {
	text_offset_.set(x, y);
	if (text) text->set_location(location.x + text_offset_.x, location.y + text_offset_.y);
}

void set_values(Vector2& vector, const string& values) {
	size_t splitter = values.find(',');
	vector.x = stof(values.substr(0, splitter));
	vector.y = stof(values.substr(splitter + 1));
}

// Add vars.
void Moveable::tick_timer(float modifier) {
	if (script == nullptr) return;
	if (script_timer_ > 0) { script_timer_ -= (1.0f / 60) * modifier; return; }
	if (script_line_ >= script->size()) return;

	vector<string> dereferenced_sript = *script;  // todo

	string line = dereferenced_sript[script_line_];
	size_t splitter = line.find(':');
	string action = line.substr(0, splitter);
	string data = line.substr(splitter + 1);
	for (auto& c : action) c = toupper(c);

	if (action == "WAIT") script_timer_ = stof(data);
	else if (action == "LOCATION") { set_values(location, data); }
	else if (action == "VELOCITY") { set_values(velocity, data); }
	else if (action == "ACCELERATION") { set_values(acceleration, data); }
	else if (action == "X") location.x = stof(data);
	else if (action == "Y") location.y = stof(data);
	else if (action == "WIDTH") size.x = stof(data);
	else if (action == "HEIGHT") size.y = stof(data);
	else if (action == "COLOUR") set_colour(Colour::hex_to_rgb(data));
	else if (action == "ALPHA") colour.set_alpha(stof(data));
	else if (action == "GRADIENT") set_gradient_colour(Colour::hex_to_rgb(data));
	else if (action == "GRADIENT_ALPHA") gradient_colour.set_alpha(stof(data));
	else if (action == "JUMP") script_line_ = stoi(data);
	else if (action == "TEXTURE") set_texture(Image::get_image(data));
	// TODO else if (action == "COLOUR_SHIFT");
	script_line_++;
}

void Moveable::load_script(const string& path) {
	log_t("Loading script... " + path + " [" + name + "]");
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

void Moveable::shift_colour(float modifier) {
	colour_shift.progress += (colour_shift.direction == ColourShift::Direction::Up ? colour_shift.speed : -colour_shift.speed) * modifier;

	if (colour_shift.progress >= 1.0f || colour_shift.progress <= 0.0f) {
		colour_shift.progress = clamp(colour_shift.progress, 0.0f, 1.0f);
		if (colour_shift.loop) {
			colour_shift.direction = (colour_shift.progress == 1.0f ? ColourShift::Direction::Down : ColourShift::Direction::Up);
		} else {
			stop_colour_shift();
			if (colour_shift.fade_to_death) {
				add_flag(DISABLED);
			}
		}
	}

	if (colour_shift.fade_to_death) {
		if (colour.get_w() <= 0) {
			add_flag(DISABLED);
			stop_colour_shift();
		}
	}

	if (colour_shift.fade_to_death && evaluated_colour.get_w() <= 0) add_flag(DISABLED);
	if (colour_shift.condition && *colour_shift.condition != colour_shift.target) stop_colour_shift();
}

void Moveable::stop_colour_shift() {
	shifting_colour_ = false;
	// if (filters.size() > 0 && filters.front()->mode == ColourFilter::Mode::Replacement) filters.pop_front();
}

void Moveable::set_colour_shift(const ColourShift& shift) {
	colour_shift = shift;
	shifting_colour_ = true;
	// if (filters.size() == 0 || filters.front()->mode != ColourFilter::Mode::Replacement) filters.push_front(new ColourFilter(Colour(), ColourFilter::Mode::Replacement));
}

void Moveable::common(float modifier) {
	tick_timer(modifier);

	evaluated_colour = colour;

	if (shifting_colour_) {
		shift_colour(modifier);
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

	if (hover_filter.enabled) {
		evaluated_colour *= hover_filter.colour;
	}
	
	if (shifting_colour_) {
		if (colour_shift.with_gradient) {
			gradient_colour = evaluated_colour;
		}
	}

}

void Moveable::update(float modifier) {
	common(modifier);

	velocity.x += acceleration.x;
	velocity.y += acceleration.y;

	location.x += velocity.x;
	location.y += velocity.y;
}

void Moveable::on_hover() {
	hover_filter.enabled = true;
	if (has_flag(BUTTON & TOOLTIP)) {
		text->remove_flag(DISABLED);
	}
}

void Moveable::on_hover_stop() {
	hover_filter.enabled = false;
	if (has_flag(BUTTON & TOOLTIP)) {
		text->add_flag(DISABLED);
	}
}
