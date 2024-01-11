#pragma once

#define RENDER_2
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

#include "assets/enemy.h"
#include "assets/player.h"
#include "assets/collidable_new.h"
#include "assets/text_new.h"
#include "assets/planet.h"

#include "shapes/square.h"

#include "effects/fire.h"
#include "effects/stars.h"
#include "effects/ice.h"

#include "tools/render_new.h"
#include "tools/loader_new.h"
#include "tools/structs.h"
#include "tools/fonts.h"
#include "tools/common.h"

#include "tools/console_new.h"
#include "tools/colour.h"

class Keyboard;
class Gamepad;
class Mouse;

using namespace std;

class GameNew {
	public:


		double game_speed = 2.5;
		int elapsed_days = 0;

		vector<NationNew*> nations;

		stringstream ss;
		UnitNew* selected_unit; ProvinceNew* selected_province;

		NationNew* player_nation = nullptr;


		const double GRAVITY = 0.004;

		Vector2 mouse_position = { -1, -1 };
		Vector2 original_position = { -1, -1 };
		Vector2 offset_position;

		Mouse* mouse;
		Keyboard* keyboard;
		Gamepad* gamepad;
		ConsoleNew* console;

		double update_time_ = 0;

		bool god_mode = true;

		GLFWwindow* window = nullptr;
		MoveableNew* selected_object = nullptr;
		TextNew* selected_button = nullptr;
		LoaderNew loader;
		Level2 current_level;
		vector<Level2> levels;

		int level_index = 0;
		double scroll_size = 1;

		bool rbDown = false, mbDown = false;

		vector<TextNew*> text_objects;
		vector<MoveableNew*> queue_objects;
		vector<MoveableNew*> objects;
		vector<map<double, int>> timers_;

		TextNew t_FPSCounter, t_PlayerLocation, t_PlayerVelocity, t_PlayerAcceleration, t_Alt, t_Alt2, t_Alt3, t_Notification, t_Hint, t_Hint2;

		RenderNew render;

		// public:
		static GameNew* game;

		bool show_console = false;

		enum Direction { LEFT = -1, RIGHT = 1, UP = 2, DOWN = 3 };
		enum Action { PRESS = -1, RELEASE = 1 };

		// Player player;

		int fpsLimit = 0;
		int update_rate = 144;

		void checkCollision();
		void checkTimers();
		void toggleConsole();
		void consoleEntry(int);
		void consoleExecute();
		void registerObject(MoveableNew*);
		void registerObject(TextNew*);
		void updateObjects(double = 1.0);
		void updateStatistics(int, int);
		void updateProperties();
		void debugMode();
		int  gameLoop();
		void fireShot();

		// Player* getPlayer();

		GameNew();
		GameNew(int, char**);

};
