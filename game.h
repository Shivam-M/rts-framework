#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "assets/collidable.h"
#include "assets/text.h"

#include "tools/render.h"
#include "tools/loader.h"
#include "tools/console.h"
#include "tools/fonts.h"

class Keyboard;
class Mouse;
class GeneralTooltip;
class Panel;

static void loadProvinceNeighbours(string);
static void loadProvinceAttributes(string);

using namespace std;

class Game {
	public:
		const float GRAVITY = 0.004f;
		float game_speed = 2.5f;
		float update_time_ = 0.f;
		int elapsed_days = 0;
		int level_index = 0;

		map<int, int> button_map; // replace
		Vector2 mouse_position;
		Vector2 original_position;
		Vector2 cursor_position;

		Render render;
		Loader loader;
		Level current_level;
		vector<Level> levels;

		vector<Text*> text_objects;
		vector<Moveable*> queue_objects;
		vector<Moveable*> objects;
		vector<Nation*> nations;

		GLFWwindow* window = nullptr;
		Moveable* hovered_object = nullptr;
		Moveable* selected_object = nullptr;
		Province* selected_province = nullptr;
		Unit* selected_unit = nullptr;
		Nation* player_nation = nullptr;

		Mouse* mouse = nullptr;
		Keyboard* keyboard = nullptr;
		Console* console = nullptr;
		GeneralTooltip* province_tooltip = nullptr;
		Panel* pause_panel = nullptr;

		Text t_FPSCounter, t_PlayerLocation, t_PlayerVelocity, t_PlayerAcceleration, t_Alt, t_Alt2, t_Alt3, t_Notification, t_Hint, t_Hint2;

		// public:
		// Player player;
		static Game* game;

		bool value_view = false;
		bool game_paused = false;
		bool picking_nation = false;
		bool show_console = false;
		bool god_mode = true;
		int fps_limit = 0;
		int update_rate = 60;
		
		void loadLevels(string level_directory);
		void registerObject(Moveable* object);
		void registerObject(Text* text_object);
		void updateObjects(float modifier = 1.0f);
		void updateStatistics(int frames, int updates);
		void updateProperties();
		void checkCollision();
		void debugMode();
		void toggleDebug();
		void fireShot();
		void pauseGame();
		void setupRTSGame();
		void moveUnit(Province* province);
		void hoverProvince(Province* province);
		void hoverUnit(Unit* unit);
		void updateCursor();
		void executeAction(ACTIONS action);
		void setButton(int button, int state) { button_map[button] = state; }
		int  getButton(int button) { return button_map[button]; }
		int  gameLoop();

		Moveable* createButton(Vector2 location, Vector2 size, Colour colour, Colour gradient, ACTIONS action, Text*);
		Moveable* getObjectUnderMouse();
		
		// Player* getPlayer();

		Game() {};
		Game(int, char**);
};
