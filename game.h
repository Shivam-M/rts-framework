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
struct ColourFilter;

class Game {
	public:
		float update_time_ = 0.f;
		float launch_time_ = 0.f;

		Vector2 mouse_position;
		Vector2 original_position;
		Vector2 cursor_position;
		Vector2 drag_offset;

		Render* render = nullptr;
		Loader* loader = nullptr;
		vector<Level*> levels;

		vector<Text*> text_objects;
		vector<Moveable*> queue_objects;
		vector<Moveable*> objects;
		Moveable* selected_object = nullptr;
		Moveable* hovered_object = nullptr;
		Moveable* dragged_object = nullptr;
		ColourFilter* global_filter;

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
		
		virtual int game_loop();
		virtual void initialise_extended();
		virtual void update_objects(float modifier = 1.0f);
		virtual void update_statistics(const int& frames, const int& updates);
		virtual void update_properties();
		virtual void update_cursor();
		virtual Moveable* get_object_under_mouse();

		bool is_cursor_on_texture(Moveable* moveable, const Vector2& cursor_position);
		void load_levels(string level_directory);
		void load_level_dynamically(const string& level_path, const string& instance_name = "");
		void register_object(Moveable* object);
		void register_object(Text* text_object);
		void debug_mode();
		void toggle_debug();
		void set_button(const int& button, const int& state) { button_map[button] = state; }
		const int& get_button(const int& button) { return button_map[button]; }

		Player* get_player() { return player; }

		Game() : Game(0, nullptr) {}
		Game(int, char**);
};
