#pragma once

#include "assets/colour.h"

#include <map>
#include <vector>
#include <string>

using namespace std;

class Moveable;
class Keyboard;
class Mouse;
class Render;
class Player;
class Console;
class Text;
class Loader;
struct Level;
struct GLFWwindow;

class Game {
	public:
		float update_time_ = 0.f;
		float launch_time_ = 0.f;

		Vector2 mouse_position;
		Vector2 original_position;
		Vector2 cursor_position;
		Vector2 drag_offset;

		Render* render = nullptr;
		Loader* loader;
		vector<Level*> levels;

		vector<Text*> text_objects;
		vector<Moveable*> queue_objects;
		vector<Moveable*> objects;
		Moveable* selected_object = nullptr;
		Moveable* hovered_object = nullptr;
		Moveable* dragged_object = nullptr;
		bool holding_left_mouse_button = false; // TODO: move to button_map

		map<int, int> button_map;

		GLFWwindow* window = nullptr;

		Mouse* mouse = nullptr;
		Keyboard* keyboard = nullptr;
		Console* console = nullptr;

		Text *t_FPSCounter,
			 *t_Information,
			 *t_Information2,
			 *t_Information3,
			 *t_Alt,
			 *t_Alt2,
			 *t_Alt3,
			 *t_Notification;

		Player* player = nullptr;

		// public:
		static Game* game;

		bool god_mode = true;
		int fps_limit = 0;
		int update_rate = 144;
		
		virtual void extendedInitialisation();
		virtual void updateObjects(const float& modifier = 1.0f);
		virtual void updateStatistics(const int& frames, const int& updates);
		virtual void updateProperties();
		void loadLevels(string level_directory);
		void registerObject(Moveable* object);
		void registerObject(Text* text_object);
		void debugMode();
		void toggleDebug();
		void updateCursor();
		void setButton(int button, int state) { button_map[button] = state; }
		const int& getButton(const int& button) { return button_map[button]; }
		virtual int gameLoop();
		virtual Moveable* getObjectUnderMouse();

		Player* getPlayer() { return player; }

		Game() {};
		Game(int, char**);
};
