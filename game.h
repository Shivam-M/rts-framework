#pragma once

#include "assets/player.h"
#include "assets/text.h"

#include "tools/render.h"
#include "tools/loader.h"
#include "tools/console.h"
#include "tools/fonts.h"
#include "tools/ui_manager.h"


class Keyboard;
class Mouse;

using namespace std;

class Game {
	public:
		float update_time_ = 0.f;
		float launch_time_ = 0.f;

		Vector2 mouse_position;
		Vector2 original_position;
		Vector2 cursor_position;
		Vector2 drag_offset;

		Render render;
		Loader loader;
		vector<Level> levels;

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

		Text t_FPSCounter,
			t_Information,
			t_Information2,
			t_Information3,
			t_Alt,
			t_Alt2,
			t_Alt3,
			t_Notification;

		Player player;

		// public:
		static Game* game;

		bool show_console = false;
		bool god_mode = true;
		int fps_limit = 0;
		int update_rate = 60;
		
		virtual void extendedInitialisation() { console->build(); }
		virtual void updateObjects(float modifier = 1.0f);
		virtual void updateStatistics(int frames, int updates);
		virtual void updateProperties();
		void loadLevels(string level_directory);
		void registerObject(Moveable* object);
		void registerObject(Text* text_object);
		void debugMode();
		void toggleDebug();
		void updateCursor();
		void setButton(int button, int state) { button_map[button] = state; }
		const int&  getButton(const int& button) { return button_map[button]; }
		virtual int gameLoop();
		virtual Moveable* getObjectUnderMouse();

		Player* getPlayer() { return &player; }

		Game() {};
		Game(int, char**);
};
