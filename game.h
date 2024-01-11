#ifndef GAME_H
#define GAME_H
#pragma once
// #include <iostream>
// #include <stdio.h>

// #include <string>
// #include <algorithm>
// #include <sstream>
// #include <vector>

#define RENDER_2
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

#include "assets/enemy.h"
#include "assets/player.h"
#include "assets/collidable.h"
#include "assets/text.h"

#include "shapes/square.h"

#include "effects/fire.h"
#include "effects/stars.h"

#include "tools/render.h"
#include "tools/loader.h"
#include "tools/structs.h"
#include "tools/fonts.h"
#include "tools/common.h"

#include "tools/TextRenderer.hpp"
#include "tools/console.h"
#include "effects/ice.h"



#include "assets/planet.h"

#include "tools/colour.h"

class Keyboard;
class Gamepad;
class Mouse;


using namespace std;

class Game {
	public: //temp. need to clean
		const int LEVEL_TRANSITION_MODE = 1;
		const double GRAVITY = 0.004;

		double mouse_position[2] = { -1, -1 };
		double original_position[2] = { -1, -1 };
		double offset_position[2] = {0,0 };

		Keyboard* keyboard;
		Gamepad* gamepad;
		Mouse* mouse;
		Console* console;

		double update_time_ = 0;

		int lives = 5;

		bool debug = false, traversed = false, god_mode = true;
		
		GLFWwindow* window = nullptr;
		
		Enemy enemy;
		Circle circle;
		Moveable* selected_object = nullptr;
		Text* selected_button = nullptr;
		Loader loader;
		Level current_level;
		vector<Level> levels;
		Ice ice;

		int level_index = 0;
		double scroll_size = 1;

		bool rbDown = false, mbDown = false;

		Collidable ground2;

		vector<Text*> text_objects;
		vector<Text*> secondary_objects;
		vector<Moveable*> queue_objects;
		vector<Moveable*> objects;
		vector<map<double, int>> timers_;

		Text t_FPSCounter, t_PlayerLocation, t_PlayerVelocity, t_PlayerAcceleration, t_Alt, t_Alt2, t_Alt3, t_Notification, t_Hint, t_Hint2;

		Render render;

	// public:
		static Game* game;

		bool show_console = false;

		enum Direction { LEFT = -1, RIGHT = 1, UP = 2, DOWN = 3};
		enum Action {PRESS = -1, RELEASE = 1};

		Player player;

		int fpsLimit = 0;
		int update_rate = 144;

		bool isWithin(Moveable*, double, double);
		bool isWithin(double, double, double, double, double, double);
		void transitionLevel();
		void checkCollision();
		void checkTimers();
		// void handleMovement(Direction, Action = PRESS);
		// static void keyCallback(GLFWwindow*, int, int, int, int);
		// static void mouseCallback(GLFWwindow*, int, int, int);
		void toggleConsole();
		void consoleEntry(int);
		void consoleExecute();
		// static void scrollCallback(GLFWwindow*, double, double);
		void selectButton(vector<Text*>, double, double);
		void registerObject(Moveable*);
		void registerObject(Text*);
		void updateObjects(double = 1.0);
		void updateStatistics(int, int);
		void updateProperties();
		void debugMode();
		int  gameLoop();
		void fireShot();

		vector<Unit*> getUnitsOnProvince(ProvinceMV* province);

		Player* getPlayer();

		Game();
		Game(int, char**);

};

#endif
