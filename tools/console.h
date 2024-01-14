#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../assets/text.h"

using namespace std;

class Game;

class Console {
	private:
		const string ALPHABET{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ" };

		static Game* game;
		Moveable entry_box, debug_box;
		Text entry_text, debug_text, feedback_text;

		vector<Moveable*> cMoveable;
		vector<Text*> cText;

		bool visible_ = false;

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
