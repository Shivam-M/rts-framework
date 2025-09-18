#include "console.h"

#include "../game.h"
#include "../assets/text_entry.h"
#include "../tools/common.h"
#include "../tools/render.h"
#include "../tools/ui_manager.h"
#include "../tools/fonts.h"

#include <set>

using namespace std;

void Console::build() {
	debug_background_ = new Moveable(Vector2(0.275, 0.4), Vector2(0.45, 0.225), Colour(20, 20, 20, 250), Colour(20, 20, 20, 250));
	debug_background_->name = "Console Debug Box";
	debug_background_->add_flag(CURVED | DISABLED | FIXED_POS);

	debug_text_ = new Text(Vector2(0.5, 0.55), Fonts::get_font("data/fonts/consolab.ttf", 12, true), Colour(22, 160, 133, 250), "");
	debug_text_->add_flag(DISABLED | FIXED_POS);
	debug_text_->alignment = Text::Alignment::Centre;

	entry_background_ = new Moveable(Vector2(0.275, 0.4), Vector2(0.45, 0.1), Colour(22, 160, 133, 250), Colour(22, 160, 133, 250));
	entry_background_->name = "Console Entry Box";
	entry_background_->add_flag(CURVED | DISABLED | FIXED_POS);

	entry_text_ = new TextEntry(Vector2(0.30, 0.47), Fonts::get_font("data/fonts/consolab.ttf", 30, true), Colour(189, 195, 199, 175), "");
	entry_text_->add_flag(TEXT | DISABLED | FIXED_POS);

	feedback_text_ = new Text(Vector2(0.3, 0.595), Fonts::get_font("data/fonts/consolab.ttf", 12, true), Colour(26, 188, 156, 250), "");
	feedback_text_->add_flag(DISABLED | FIXED_POS);

	reg(debug_background_);
	reg(debug_text_);
	reg(entry_background_);
	reg(entry_text_);
	reg(feedback_text_);
}

void Console::reg(Moveable* moveable) {
	bundle_.emplace_back(moveable);
	if (moveable->has_flag(TEXT))
		game->register_object(static_cast<Text*>(moveable));
	else
		game->register_object(moveable);
}

bool Console::visible() const {
	return visible_;
}

void Console::toggle() {
	visible_ = !visible_;
	for (Moveable* moveable : bundle_) visible_ ? moveable->remove_flag(DISABLED) : moveable->add_flag(DISABLED);
}

void Console::entry(const int& character) {
	if (!(set<int> {96}).count(character)) // '`'
		entry_text_->input(character);
}

void Console::feedback(const string& message) {
	feedback_text_->set_content(message);
	log_t(message);
}

void Console::update(const string& message) {
	debug_text_->set_content(message);
}

void lower(string& text) {
	for (char& c : text) {
		if (c >= 'A' && c <= 'Z') {
			c += 32;
		}
	}
}

void Console::execute() {
	string command = entry_text_->get_content(true), temp;
	stringstream ss(command);
	vector<string> args;

	while (getline(ss, temp, ' ')) args.emplace_back(temp);

	try {
		if (args.empty()) return;
		string cmd = args[0];
		if (cmd == "FPSLIMIT" && args.size() > 1) {
			game->fps_limit = stoi(args[1]);
			feedback("Changed FPS limit to " + args[1]);
		}
		else if (cmd == "RENDER" && args.size() > 1) {
			game->render->render_level = stoi(args[1]);
			feedback("Updated rendering level to " + args[1]);
		}
		else if (cmd == "GODMODE" && args.size() > 1) {
			game->god_mode = stoi(args[1]);
			feedback("Set god mode to " + args[1]);
		}
		else if (cmd == "UPDATE" && args.size() > 1) {
			game->update_rate = stoi(args[1]);
			feedback("Set update rate to " + args[1]);
		}
		else if (cmd == "RESET") {
			game->render->offsets = { 0, 0 };
			feedback("Reset view offsets");
		}
		else if (cmd == "SHOW" && args.size() > 1) {
			lower(args[1]);
			UIManager::show(args[1]);
		}
		else if (cmd == "HIDE" && args.size() > 1) {
			lower(args[1]);
			UIManager::hide(args[1]);
		}
		else if (cmd == "TOGGLE" && args.size() > 1) {
			lower(args[1]);
			UIManager::toggle(args[1]);
		}
		else if (cmd == "SCRIPT" && args.size() > 1 && game->selected_object) {  // todo: make path safer 
			game->selected_object->load_script("data/scripts/" + args[1]);
		}
		else if (cmd == "VY" && args.size() > 1 && game->selected_object) {
			game->selected_object->velocity.y = stod(args[1]);
			feedback("Set Y velocity of " + game->selected_object->name + " to " + args[1]);
		}
		else if (cmd == "DUMP") {
			for (Moveable* m : game->objects)
				log_t(m->name, CON_RED ": " CON_NORMAL, m->get_location().x, ", ", m->get_location().y);
		}
		else if (cmd == "DEBUG") {
			if (game->selected_object) {
				game->selected_object->filters.push_back(new ColourFilter(Colour(0.5, 0.5, 0.5, 100), ColourFilter::Mode::Multiplication));
			}
		}
		else {
			feedback("Invalid command or usage: " + command);
		}
	}
	catch (...) {
		feedback("Failed to execute command: " + command);
	}
}
