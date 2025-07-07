#include "console.h"
#include "fonts.h"
#include "../game.h"

Game* Console::game = nullptr;

using namespace std;

Console::Console(Game* g) {
	game = g;
}

void Console::build() {
	entry_box = Moveable();
	entry_box.setColour(Colour(22, 160, 133, 250));
	entry_box.setGradientColour(Colour(22, 160, 133, 250));
	entry_box.setLocation(0.275, 0.4);
	entry_box.setSize(0.45, 0.1);
	entry_box.addFlag(CURVED | DISABLED | FIXED_POS);

	entry_text = TextEntry(Vector2(0.30, 0.47), Fonts::getFont(CONSOLAS_BOLD, 30), Colour(189, 195, 199, 175), "");
	entry_text.addFlag(DISABLED | FIXED_POS);

	debug_box.setColour(Colour(20, 20, 20, 250));
	debug_box.setGradientColour(Colour(20, 20, 20, 250));
	debug_box.setLocation(0.275, 0.4);
	debug_box.setSize(0.45, 0.225);
	debug_box.addFlag(CURVED | DISABLED | FIXED_POS);

	debug_text = Text(Vector2(0.5, 0.55), Fonts::getFont(CONSOLAS_BOLD, 12), Colour(22, 160, 133, 250), "");
	debug_text.addFlag(DISABLED | FIXED_POS);
	debug_text.setAlignment(CENTRE);

	feedback_text = Text(Vector2(0.3, 0.595), Fonts::getFont(CONSOLAS_BOLD, 12), Colour(26, 188, 156, 250), "");
	feedback_text.addFlag(DISABLED | FIXED_POS);

	rMoveable(&debug_box);
	rMoveable(&entry_box);

	rText(&entry_text);
	rText(&debug_text);
	rText(&feedback_text);
}

void Console::rText(Text* text) {
	text->addFlag(DEBUG);
	cText.push_back(text);
	game->registerObject(text);
}

void Console::rMoveable(Moveable* moveable) {
	moveable->addFlag(DEBUG);
	cMoveable.push_back(moveable);
	game->registerObject(moveable);
}

bool Console::visible() {
	return visible_;
}

void Console::toggle() {
	visible_ = !visible_;
	for (Moveable* moveable : cMoveable) visible_ ? moveable->removeFlag(DISABLED) : moveable->addFlag(DISABLED);
	for (Text* text : cText) visible_ ? text->removeFlag(DISABLED) : text->addFlag(DISABLED);
}

void Console::entry(int character) {
	entry_text.input(character);
}

void Console::feedback(string message) {
	feedback_text.setContent(message);
	log_t(message);
}

void Console::update(string message) {
	debug_text.setContent(message);
}

void Console::execute() {
	string command = entry_text.getContent(), temp;
	stringstream ss(command);
	vector<string> args;

	while (getline(ss, temp, ' ')) args.push_back(temp);

	try {
		if (args[0] == "FPSLIMIT") {
			game->fps_limit = stoi(args[1]);
			feedback("Changed FPS limit to " + args[1]);
		}
		else if (args[0] == "RENDER") {
			game->render.setRenderLevel(stoi(args[1]));
			feedback("Updated rendering level to " + args[1]);
		}
		else if (args[0] == "GODMODE") {
			game->god_mode = stoi(args[1]);
			feedback("Set god mode to " + args[1]);
		}
		else if (args[0] == "UPDATE") {
			game->update_rate = stoi(args[1]);
			feedback("Set update rate to " + args[1]);
		}
		else if (args[0] == "RESET") {
			game->render.offsets = { 0, 0 };
			feedback("Reset view offsets");
		}
		else if (args[0] == "SHOW") {
			UIManager::Show(args[1]);
		}
		else if (args[0] == "HIDE") {
			UIManager::Hide(args[1]);
		}
		else if (args[0] == "TOGGLE") {
			UIManager::Toggle(args[1]);
		}
		else if (args[0] == "SCRIPT") {
			game->selected_object->loadScript("data/scripts/" + args[1]);  // todo: make safer
		}
		else if (args[0] == "VY") {
			if (game->selected_object) {
				game->selected_object->velocity.y = stod(args[1]);
				feedback("Set Y velocity of " + game->selected_object->getName() + " to " + args[1]);
			}
		}
		else if (args[0] == "DUMP") {
			for (Moveable* m : game->objects) {
				log_t(m->getName(), CON_RED ": " CON_NORMAL, m->getLocation().x, ", ", m->getLocation().y);
			}
		}
		else {
			feedback("Invalid command: " + command);
		}
	}
	catch (...) {
		feedback("Failed to execute command: " + command);
	}
}
