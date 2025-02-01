#pragma once
#include "../assets/text_entry.h"

using namespace std;

class Game;

class Console {
	private:
		const string ALPHABET{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
		static Game* game;

		bool visible_ = false;

		Moveable entry_box, debug_box;
		Text debug_text, feedback_text;
		TextEntry entry_text;
		vector<Moveable*> cMoveable;
		vector<Text*> cText;

	public:
		Console(Game*);

		void build();
		void toggle();
		void execute();
		bool visible();

		void entry(int);
		void feedback(string);
		void update(string);

		void rText(Text*);
		void rMoveable(Moveable*);
};
