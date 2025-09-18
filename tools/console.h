#pragma once

#include <string>
#include <vector>

using namespace std;

class Game;
class TextEntry;
class Moveable;
class Text;

class Console {
	private:
		Game* game = nullptr;
		Moveable* entry_background_ = nullptr;
		Moveable* debug_background_ = nullptr;
		Text* debug_text_ = nullptr;
		Text* feedback_text_ = nullptr;
		TextEntry* entry_text_ = nullptr;
		vector<Moveable*> bundle_;
		bool visible_ = false;

	public:
		Console(Game* instance) : game(instance) {}

		void build();
		void toggle();
		void execute();
		bool visible() const;

		void entry(const int& character);
		void feedback(const string& text);
		void update(const string& text);
		void reg(Moveable* moveable);
};
