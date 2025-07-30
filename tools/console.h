#pragma once

class Game;
class TextEntry;

class Console {
	private:
		Game* game = nullptr;
		Moveable* entry_box = nullptr;
		Moveable* debug_box = nullptr;
		Text* debug_text = nullptr;
		Text* feedback_text = nullptr;
		TextEntry* entry_text = nullptr;
		vector<Moveable*> console_moveables;
		bool visible_ = false;

	public:
		Console(Game* g) : game(g) {}

		void build();
		void toggle();
		void execute();
		bool visible() const;

		void entry(const int& character);
		void feedback(const string& text);
		void update(const string& text);
		void reg(Moveable* moveable);
};
