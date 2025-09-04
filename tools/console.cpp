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
	debug_box = new Moveable(Vector2(0.275, 0.4), Vector2(0.45, 0.225), Colour(20, 20, 20, 250), Colour(20, 20, 20, 250));
	debug_box->setName("Console Debug Box");
	debug_box->addFlag(CURVED | DISABLED | FIXED_POS);

	debug_text = new Text(Vector2(0.5, 0.55), Fonts::getFont("data/fonts/consolab.ttf", 12, true), Colour(22, 160, 133, 250), "");
	debug_text->addFlag(DISABLED | FIXED_POS);
	debug_text->setAlignment(CENTRE);

	entry_box = new Moveable(Vector2(0.275, 0.4), Vector2(0.45, 0.1), Colour(22, 160, 133, 250), Colour(22, 160, 133, 250));
	entry_box->setName("Console Entry Box");
	entry_box->addFlag(CURVED | DISABLED | FIXED_POS);

	entry_text = new TextEntry(Vector2(0.30, 0.47), Fonts::getFont("data/fonts/consolab.ttf", 30, true), Colour(189, 195, 199, 175), "");
	entry_text->addFlag(TEXT | DISABLED | FIXED_POS);

	feedback_text = new Text(Vector2(0.3, 0.595), Fonts::getFont("data/fonts/consolab.ttf", 12, true), Colour(26, 188, 156, 250), "");
	feedback_text->addFlag(DISABLED | FIXED_POS);

	reg(debug_box);
	reg(debug_text);
	reg(entry_box);
	reg(entry_text);
	reg(feedback_text);
}

void Console::reg(Moveable* moveable) {
	console_moveables.emplace_back(moveable);
	if (moveable->hasFlag(TEXT))
		game->registerObject(static_cast<Text*>(moveable));
	else
		game->registerObject(moveable);
}

bool Console::visible() const {
	return visible_;
}

void Console::toggle() {
	visible_ = !visible_;
	for (Moveable* moveable : console_moveables) visible_ ? moveable->removeFlag(DISABLED) : moveable->addFlag(DISABLED);
}

void Console::entry(const int& character) {
	if (!(set<int> {96}).count(character)) // '`'
		entry_text->input(character);
}

void Console::feedback(const string& message) {
	feedback_text->setContent(message);
	log_t(message);
}

void Console::update(const string& message) {
	debug_text->setContent(message);
}

void lower(string& text) {
	for (char& c : text) {
		if (c >= 'A' && c <= 'Z') {
			c += 32;
		}
	}
}

void Console::execute() {
	string command = entry_text->getContent(true), temp;
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
			game->render->setRenderLevel(stoi(args[1]));
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
			UIManager::Show(args[1]);
		}
		else if (cmd == "HIDE" && args.size() > 1) {
			lower(args[1]);
			UIManager::Hide(args[1]);
		}
		else if (cmd == "TOGGLE" && args.size() > 1) {
			lower(args[1]);
			UIManager::Toggle(args[1]);
		}
		else if (cmd == "SCRIPT" && args.size() > 1 && game->selected_object) {  // todo: make path safer 
			game->selected_object->loadScript("data/scripts/" + args[1]);
		}
		else if (cmd == "VY" && args.size() > 1 && game->selected_object) {
			game->selected_object->velocity.y = stod(args[1]);
			feedback("Set Y velocity of " + game->selected_object->getName() + " to " + args[1]);
		}
		else if (cmd == "DUMP") {
			for (Moveable* m : game->objects)
				log_t(m->getName(), CON_RED ": " CON_NORMAL, m->getLocation().x, ", ", m->getLocation().y);
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
