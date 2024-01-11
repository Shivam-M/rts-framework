#pragma once
#include <functional>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../assets/moveable_new.h"
#include "../assets/text_new.h"

using namespace std;

class GameNew;

class ConsoleNew {
private:
	const string ALPHABET{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ" };

	static GameNew* game;
	MoveableNew entry_box, debug_box;
	TextNew entry_text, debug_text, feedback_text;

	vector<MoveableNew*> cMoveable;
	vector<TextNew*> cText;

	bool visible_ = false;

public:
	ConsoleNew(GameNew*);

	void build();
	void toggle();
	void execute();
	bool visible();

	void entry(int);
	void feedback(string);
	void update(string);

	void rText(TextNew*);
	void rMoveable(MoveableNew*);
};
