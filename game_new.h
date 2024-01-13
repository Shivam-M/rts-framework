#pragma once

#define RENDER_2
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>

#include "assets/collidable_new.h"
#include "assets/text_new.h"

#include "tools/render_new.h"
#include "tools/loader_new.h"
#include "tools/console_new.h"
#include "tools/fonts.h"

class KeyboardNew;
class Gamepad;
class MouseNew;

using namespace std;

class GameNew {
	public:
		float game_speed = 2.5;
		int elapsed_days = 0;
		map<int, int> button_map;

		vector<NationNew*> nations;

		stringstream ss;
		UnitNew* selected_unit; ProvinceNew* selected_province;

		NationNew* player_nation = nullptr;

		const float GRAVITY = 0.004;

		Vector2 mouse_position;
		Vector2 original_position;

		MouseNew* mouse;
		KeyboardNew* keyboard;
		Gamepad* gamepad;
		ConsoleNew* console;

		float update_time_ = 0;

		bool god_mode = true;

		GLFWwindow* window = nullptr;
		MoveableNew* selected_object = nullptr;
		LoaderNew loader;
		Level2 current_level;
		vector<Level2> levels;

		int level_index = 0;
		float scroll_size = 1;

		bool rbDown = false, mbDown = false;

		vector<TextNew*> text_objects;
		vector<MoveableNew*> queue_objects;
		vector<MoveableNew*> objects;
		vector<map<float, int>> timers_;

		TextNew t_FPSCounter, t_PlayerLocation, t_PlayerVelocity, t_PlayerAcceleration, t_Alt, t_Alt2, t_Alt3, t_Notification, t_Hint, t_Hint2;

		RenderNew render;

		// public:
		static GameNew* game;

		bool show_console = false;

		enum Direction { LEFT = -1, RIGHT = 1, UP = 2, DOWN = 3 };
		enum Action { PRESS = -1, RELEASE = 1 };

		// Player player;

		int fps_limit = 0;
		int update_rate = 144;
		
		void registerObject(MoveableNew*);
		void registerObject(TextNew*);
		void updateObjects(float = 1.0);
		void updateStatistics(int, int);
		void updateProperties();
		void checkCollision();
		void debugMode();
		void fireShot();
		void setButton(int button, int state) { button_map[button] = state; }
		int  getButton(int button) { return button_map[button]; }
		int  gameLoop();
		
		// Player* getPlayer();

		GameNew();
		GameNew(int, char**);
};
