#pragma once

#define RENDER_2
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>

#include "assets/collidable.h"
#include "assets/text.h"

#include "tools/render.h"
#include "tools/loader.h"
#include "tools/console.h"
#include "tools/fonts.h"

class Keyboard;
class Gamepad;
class Mouse;

using namespace std;

class Game {
	public:
		float game_speed = 2.5;
		int elapsed_days = 0;
		map<int, int> button_map;

		vector<NationNew*> nations;

		stringstream ss;
		Unit* selected_unit; Province* selected_province;

		NationNew* player_nation = nullptr;

		const float GRAVITY = 0.004;

		Vector2 mouse_position;
		Vector2 original_position;

		Mouse* mouse;
		Keyboard* keyboard;
		Gamepad* gamepad;
		Console* console;

		float update_time_ = 0;

		bool god_mode = true;

		GLFWwindow* window = nullptr;
		Moveable* selected_object = nullptr;
		Loader loader;
		Level current_level;
		vector<Level> levels;

		int level_index = 0;
		float scroll_size = 1;

		vector<Text*> text_objects;
		vector<Moveable*> queue_objects;
		vector<Moveable*> objects;
		vector<map<float, int>> timers_;

		Text t_FPSCounter, t_PlayerLocation, t_PlayerVelocity, t_PlayerAcceleration, t_Alt, t_Alt2, t_Alt3, t_Notification, t_Hint, t_Hint2;

		Render render;

		// public:
		static Game* game;

		bool show_console = false;

		enum Direction { LEFT = -1, RIGHT = 1, UP = 2, DOWN = 3 };
		enum Action { PRESS = -1, RELEASE = 1 };

		// Player player;

		int fps_limit = 0;
		int update_rate = 144;
		
		void registerObject(Moveable*);
		void registerObject(Text*);
		void updateObjects(float = 1.0);
		void updateStatistics(int, int);
		void updateProperties();
		void checkCollision();
		void debugMode();
		void fireShot();
		void pauseGame();
		void setupRTSGame();
		void setButton(int button, int state) { button_map[button] = state; }
		int  getButton(int button) { return button_map[button]; }
		int  gameLoop();
		
		// Player* getPlayer();

		Game() {};
		Game(int, char**);
};
