#ifdef _WIN32
#include <windows.h>
#include <Psapi.h>
#endif

#include "game_rts.h"

#include "assets/province.h"
#include "assets/unit.h"
#include "assets/nation.h"
#include "assets/text.h"
#include "assets/panel.h"

#include "io/mouse.h"
#include "io/keyboard.h"
#include "io/audio.h"

#include "tools/text_renderer.h"
#include "tools/loader.h"
#include "tools/image.h"
#include "tools/fonts.h"
#include "tools/ui_manager.h"
#include "tools/common.h"
#include "tools/render.h"
#include "tools/console.h"

#include <filesystem>

#define MAX_RESTRICTED_GAME_SPEED 2000
// #define DEBUG_PROFILING

using namespace std;
namespace fs = filesystem;

GameRTS* GameRTS::instance = nullptr;

GameRTS::GameRTS() : Game() { instance = this; }
GameRTS::GameRTS(int a, char** b) : Game(a, b) { instance = this; }

void GameRTS::initialise_extended() {
	// Image::load_map("data/world_map.bmp", "data/province_colours.txt");
#define dbg2
#ifdef dbg
	load_levels("data/levels/debug/");
#else
	load_levels("data/levels/rts/");
#endif
	load_levels("data/levels/ui/");
	load_province_neighbours("data/generated/province_neighbours.txt");
	load_province_attributes("data/generated/province_dimensions.txt");
	initialise_rts_game();

	console->build();

	UIManager::hide("ui_menu_pause");
	UIManager::hide("ui_nation_tooltip");
	UIManager::hide("ui_province_tooltip");
	UIManager::hide("ui_unit_tooltip");
	UIManager::hide("ui_war_declaration");
	UIManager::hide("ui_event_choice");
	UIManager::hide("ui_war_indicator");
	UIManager::hide("ui_unit_hire");
	UIManager::hide("ui_battle_unit");
	// UIManager::hide("ui_information_header");

	cursor_ = new Moveable();
	cursor_->set_texture(Image::get_image("data/images/cursor.png"));
	cursor_->set_size(0.04 * (9 / 16.0), 0.04);
	cursor_->add_flag(FIXED_POS | CURSOR);

	render->cursor = cursor_;
	register_object(cursor_);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	log_t("Took " CON_RED, glfwGetTime() - launch_time_, " seconds " CON_NORMAL "to completely load the game.");
}

void GameRTS::initialise_rts_game() {
	execute_action(ToggleMapView);
	Loader::get_unit_map()[1]->set_path(Province::find_shortest_path(Loader::get_province_map()[5], Loader::get_province_map()[18]));

	for (const auto& u : Loader::get_unit_map()) {
		Unit* unit = u.second;
		unit->initialise();
	}

	for (const auto& n : Loader::get_nation_map()) {
		Nation* nation = n.second;
		nations.push_back(nation);
	}

	player_nation = nations[0];
	for (Moveable* moveable : objects) {
		if (moveable->has_flag(THEMED)) {
			moveable->set_colour(player_nation->colour.with_alpha(moveable->colour.a));
		}
	}
}

void load_province_neighbours(string neighbours) {
	ifstream neighbours_file(neighbours);
	if (!neighbours_file.is_open()) {
		log_t(CON_RED "Error loading province neighbour pairs file." CON_NORMAL);
		return;
	}

	string line;
	while (getline(neighbours_file, line)) {
		istringstream iss(line);
		int id, other_id;
		char comma;

		if (iss >> id >> comma >> other_id) {
			auto& province_map = Loader::get_province_map();
			Province* province = province_map[id];
			Province* other_province = province_map[other_id];

			province->register_neighbour(other_province);
			other_province->register_neighbour(province);

			log_t("Assigned province " CON_RED, province->name, CON_NORMAL " (" CON_RED, province->identifier, CON_NORMAL ") as a neighbour with " CON_RED, other_province->name, CON_NORMAL " (" CON_RED, other_province->identifier, CON_NORMAL ")");
		}
	}
}

void load_province_attributes(string attributes) {
	ifstream dimensions_file(attributes);
	if (!dimensions_file.is_open()) {
		log_t(CON_RED "ERROR! Failed to load province map data file.");
		return;
	}

	int id;
	char comma;
	float x, y, w, h, xoffset = -0.2f, yoffset = -0.1f;
	string line;
	while (getline(dimensions_file, line)) {
		istringstream iss(line);
		if (iss >> id >> comma >> x >> comma >> y >> comma >> w >> comma >> h) {
			Loader::get_province_map()[id]->set_size(w, h);
			Loader::get_province_map()[id]->set_location(x + xoffset, y + yoffset);
		}
	}
	log_t("Loaded map position data for all provinces.");
}

void GameRTS::pause_game() {
	game_paused = !game_paused;
	simulation_paused = !simulation_paused;
	UIManager::toggle("ui_menu_pause", true);
	Sound* sound = new Sound("data/click.wav");
	sound->play(0.025f);
}

void GameRTS::update_statistics(int frames, int updates) {
	Game::update_statistics(frames, updates);

	string nation_string = "-- [X]";
	ostringstream nation_treasury;
	if (player_nation) {
		nation_string = player_nation->name + " [" + to_string(player_nation->identifier) + "]";

		t_Information2->set_content(format("Money: {:.2f}", player_nation->money));
		t_Information3->set_colour(player_nation->colour);
	}

	t_Information->set_content(date.format());
	t_Information3->set_content("Playing as: " + nation_string);
}

static bool within(const Vector2& location, const Vector2& size, const Vector2& point) {
	return point.x > location.x && point.x < location.x + size.x && point.y > location.y && point.y < location.y + size.y;
}

void GameRTS::expand_nation(Province* province) {
	viewed_nation = province->nation;
	UIManager::show("ui_nation_tooltip");
}

void GameRTS::move_unit(Province* province) {
	Unit* unit = reinterpret_cast<Unit*>(selected_object);
	if (unit->state != Unit::State::Fighting && unit->state != Unit::State::Dead ) {
		unit->set_path(Province::find_shortest_path(unit->province, province));
		unit->initialise();
	}
}

Moveable* GameRTS::get_object_under_mouse() {
	Moveable* object = nullptr;
	for (Moveable* moveable : game->objects) {
		if (moveable->get_flags() & (UNEDITABLE | DISABLED)) continue;
		if (!game_paused) moveable->on_hover_stop();

		if (moveable->has_flag(PROVINCE)) {
			if (!is_cursor_on_texture(moveable, cursor_position)) continue;
		}

		Vector2 location, size;
		if (moveable->has_flag(FIXED_POS)) {
			location = moveable->get_location();
			size = moveable->get_size();
		} else {
			location = moveable->get_location() * render->scale + render->offsets;
			size = moveable->get_size() * render->scale;
		}

		if (within(location, size, cursor_position)) {
			object = moveable;
		}
	}

	UIManager::hide("ui_province_tooltip");
	UIManager::hide("ui_unit_tooltip");

	if (object && !object->has_flag(DISABLED)) {
		if (object->has_flag(BUTTON)) {
			object->on_hover();
		}

		if (object->has_flag(PROVINCE)) {
			Province* province = reinterpret_cast<Province*>(object);

			if (!picking_nation) {
				object->on_hover();
				hover_province(province);
			} else {
				object->on_hover_stop();
				for (Province* province : province->nation->get_provinces()) province->on_hover();
			}
		} else if (object->has_flag(UNIT)) {
			hover_unit(reinterpret_cast<Unit*>(object));
		}
	}
	
	hovered_object = object;
	return hovered_object;
}

void GameRTS::hover_province(Province* province) {
	UIManager::assign_values("ui_province_tooltip", province);
	UIManager::show("ui_province_tooltip");
}

void GameRTS::hover_unit(Unit* unit) {
	UIManager::assign_values("ui_unit_tooltip", unit);
	UIManager::show("ui_unit_tooltip");
}

void GameRTS::update_cursor() {  // tied to update rate
	Game::update_cursor();
	cursor_->location = cursor_position;
}

void GameRTS::execute_action(int action, Moveable* button) {  // keep option for action-only
	static int index_font;
	static vector<string> files_font;
	Unit* hired_unit;
	War* war;
	Level* level = nullptr;

	log_t("Executing button action: " CON_RED, action);
	switch (action) {
		case ToggleControls:
			game->mouse->debug_control_scheme ^= true;
			t_Notification->set_content(format("Set debug control mode to: {}", game->mouse->debug_control_scheme));
			break;
		case PauseGame:
			pause_game();
			break;
		case PauseSimulation:
			simulation_paused = !simulation_paused;
			break;
		case SwitchNation:
			picking_nation ^= true;
			t_Notification->set_content("Select a nation to control it");
			break;
		case ToggleMapView: // Change to a mask
			value_view ^= true;
			// queue action to be executed in getObjectUnderMouse() instead of cycling through moveables again?
			for (Moveable* moveable : objects) {
				if (moveable->has_flag(PROVINCE)) {
					moveable->set_colour(moveable->colour.with_alpha((!value_view ? 200 : 75 + (reinterpret_cast<Province*>(moveable)->value) * 200)));
				}
			}
			break;
		case SaveGame:
			Loader::write_objects(objects, text_objects);
			break;
		case HireUnit:
			player_nation->money = 9999999.f;
			hired_unit = player_nation->hire_unit(999, 5.f);
			if (hired_unit) {
				Font* font = Fonts::get_font("data/fonts/Cinzel-Bold.ttf", 16, true); // (189, 195, 199, 250)
				Text* unit_text = new Text(hired_unit->get_location(), font, Colour(220, 221, 225, 200), hired_unit->name, 0.5f);
				unit_text->alignment = Text::Alignment::Centre;
				unit_text->add_flag(TEXT_BACKGROUND | UNSAVEABLE);
				unit_text->remove_flag(FIXED_POS);
				unit_text->set_background(new Moveable());
				hired_unit->name = player_nation->name + " Hired Unit";
				hired_unit->set_size(0.02125f, 0.0375f);
				hired_unit->text = unit_text;
				hired_unit->set_text_offset(0.f, -0.0025f);
				hired_unit->initialise();
				register_object(hired_unit);
				register_object(unit_text);
				register_object(unit_text->get_background());
			}
			break;
		case DebugFonts:
			if (files_font.size() == 0) {
				string path = "./data/fonts";
				for (const auto& file : fs::directory_iterator(path))
					files_font.push_back(file.path().string());
			}

			if (index_font > files_font.size() - 1) index_font = 0;
			for (const auto& u : Loader::get_unit_map()) {
				Unit* unit = u.second;
				unit->text->set_font(Fonts::get_font(files_font[index_font], 16, true));
			}
			
			t_Notification->set_content("Set game font to " + files_font[index_font]);
			index_font++;
			break;
		case ToggleTooltip:
			UIManager::toggle("ui_nation_tooltip");
			break;
		case ToggleDebugUI:
			UIManager::toggle("ui_war_indicator");
			// UIManager::toggle("ui_war_declaration");
			UIManager::toggle("ui_event_choice");
			UIManager::toggle("ui_unit_hire");
			UIManager::toggle("ui_information_header");
			break;
		case DeclareWar:
			war = new War();
			war->attacker = player_nation;
			war->defender = viewed_nation;
			war->war_goal = TAKE_KEY_PROVINCE;
			war->war_goal_target.target_province = viewed_nation->get_capital();
			register_event(WAR_START, war);
			// UIManager::assign_values("ui_war_declaration", &war);
			UIManager::hide("ui_nation_tooltip");
			// UIManager::show("ui_war_declaration");
			break;
		case Debug:
			load_level_dynamically("data/levels/ui/10-ui-battle-unit.json", "D38UG");
			UIManager::show("ui_battle_unit-D38UG");
			break;
		default:
			t_Notification->set_content(format("Unsupported button action: {}", (int)action));
	}
}

void GameRTS::register_event(Event event, void* details) {
	BattleInformation* battle;
	War* war;
	switch (event) {
		case BATTLE_START:
			battle = static_cast<BattleInformation*>(details);
			load_level_dynamically("data/levels/ui/10-ui-battle-unit.json", to_string(battle->battle_id));
			UIManager::get_panel("ui_battle_unit-" + to_string(battle->battle_id))->set_location(battle->province->get_location().x - (battle->province->get_size().x), battle->province->get_location().y);
			battles.push_back(battle);
			log_t("Started battle " CON_RED, battle->battle_id, CON_NORMAL " between " CON_RED, battle->attacker_units[0]->name, CON_NORMAL " and " CON_RED, battle->defender_units[0]->name, CON_NORMAL);
			break;
		case BATTLE_END:
			battle = static_cast<BattleInformation*>(details);
			UIManager::hide("ui_battle_unit-" + to_string(battle->battle_id));
			battles.erase(remove(battles.begin(), battles.end(), battle), battles.end());
			log_t("Ending battle " CON_RED, battle->battle_id, CON_NORMAL " between " CON_RED, battle->attacker_units[0]->name, CON_NORMAL " and " CON_RED, battle->defender_units[0]->name, CON_NORMAL);
			delete battle;
			break;
		case WAR_START:
			war = static_cast<War*>(details);
			load_level_dynamically("data/levels/ui/06-ui-war-declaration.json", to_string(war->war_id));
			// UIManager::get_panel("ui_battle_unit-" + to_string(battle->battle_id))->set_location(battle->province->get_location().x - (battle->province->get_size().x), battle->province->get_location().y);
			wars.push_back(war);
			log_t("Started war " CON_RED, war->war_id, CON_NORMAL " between " CON_RED, war->attacker->name, CON_NORMAL " and " CON_RED, war->defender->name, CON_NORMAL);
			break;
		case WAR_END:
			break;
		default:
			log_t("Unsupported event: " CON_RED, event);
			break;
	}
}

void GameRTS::update_properties() {
	get_object_under_mouse();

	if (viewed_nation)
		UIManager::assign_values("ui_nation_tooltip", viewed_nation);
	
	for (BattleInformation* battle: battles) {
		UIManager::assign_values("ui_battle_unit-" + to_string(battle->battle_id), battle, &UIManager::map_battle);
	}

	for (War* war : wars) {
		UIManager::assign_values("ui_war_declaration-" + to_string(war->war_id), war, &UIManager::map_war_declaration);
	}

	if (get_button(GLFW_MOUSE_BUTTON_MIDDLE)) {
		render->offsets.x += (cursor_position.x - original_position.x) * 0.01f;
		render->offsets.y += (cursor_position.y - original_position.y) * 0.01f;
	}

	if (!selected_object) return;

	if (get_button(GLFW_MOUSE_BUTTON_LEFT)) {
		Moveable* draggable_panel = selected_object;

		while (draggable_panel && draggable_panel->parent && !(draggable_panel->has_flag(DRAGGABLE))) {
			draggable_panel = draggable_panel->parent;
		}

		if (draggable_panel and draggable_panel->has_flag(DRAGGABLE)) {
			if (draggable_panel != dragged_object) {
				drag_offset = cursor_position - draggable_panel->location;
				dragged_object = draggable_panel;
			}
			Vector2 loc = cursor_position - drag_offset;
			draggable_panel->set_location(loc.x, loc.y);
		} else {
			dragged_object = nullptr;
		}
	} else {
		dragged_object = nullptr;
	}

	if (selected_object->has_flag(PROVINCE) && picking_nation) {
		player_nation = reinterpret_cast<Province*>(selected_object)->nation;
		picking_nation = false;

		for (Moveable* moveable : objects) {
			if (moveable->has_flag(THEMED)) {
				moveable->set_colour(player_nation->colour.with_alpha(moveable->colour.a));
			}
		}

		log_t("Player is now controlling nation " CON_RED, player_nation->name, CON_NORMAL " (" CON_RED, player_nation->identifier, CON_NORMAL ")");
	}

	if (mouse->debug_control_scheme) {
		if (selected_object->has_flag(PROVINCE)) {
			selected_object->set_colour(Colour(255, 255, 0, 80));
			for (Province* neighbour : reinterpret_cast<Province*>(selected_object)->get_neighbours()) {
				neighbour->set_colour(Colour(255, 0, 255, 80));
			}
		}
		if (get_button(GLFW_MOUSE_BUTTON_RIGHT)) {
			Vector2 new_size = Vector2(abs(game->mouse_position.x - cursor_position.x), abs(game->mouse_position.y - cursor_position.y));
			selected_object->set_size(new_size.x, new_size.y);
			t_Notification->set_content("Set size of " + game->selected_object->name + " to " + to_string(new_size.x) + ", " + to_string(new_size.y));
		}
	}

	if (selected_object->has_flag(BUTTON)) {
		execute_action(selected_object->get_button_action(), selected_object);
		selected_object = nullptr;
	}
}


void GameRTS::update_objects(float modifier) {
    // sort(objects.begin(), objects.end(), [](Moveable* a, Moveable* b) {
	//		  return a->get_priority() < b->get_priority();
	// }); // Temp, use z values for priorities instead in a Vector3
	Game::update_objects(modifier);

	HeaderInformation header_information = { player_nation->money, date.format()};

	if (!UIManager::get_panel("ui_information_header")->has_flag(DISABLED))
		UIManager::assign_values("ui_information_header", &header_information);
}

int GameRTS::game_loop() {
	int frames = 0, updates = 0, frame_count = 0;
	float average_frames = 0.f;
	float limit = 1.0f / update_rate;
	float last_time = static_cast<float>(glfwGetTime()), timer = last_time;
	float delta_time = 0.f, current_time = 0.f;
	float last_frame_time = static_cast<float>(glfwGetTime());
	float update_time;
	float second_time;

#ifdef DEBUG_PROFILING
	vector<float> temp_profiling_u;
	vector<float> temp_profiling_t;
	vector<float> temp_profiling_s;
	fps_limit = 0;
#endif

	while (!glfwWindowShouldClose(window)) {

		current_time = glfwGetTime();
		limit = 1.0f / update_rate;
		delta_time += (current_time - last_time) / limit;
		last_time = current_time;

		while (delta_time >= 1.0f) {
			update_time = glfwGetTime();
			if (!game_paused && !simulation_paused) {
				for (Nation* nation : nations)
					nation->evaluate();
				date.increment();
			}
			update_objects(60.0f / update_rate);
			update_properties();
			updates++;
			delta_time--;
			update_time_ = glfwGetTime() - update_time;
			glfwPollEvents();
		}

		if (fps_limit == 0 || glfwGetTime() - last_frame_time >= (1.0f / fps_limit)) {
			last_frame_time = glfwGetTime();
			render->render_window();
			update_cursor();
			frames++;
		}

		second_time = glfwGetTime() - timer;
		if (second_time > 1.0f) {
			// timer += second_time;
			timer++;
			average_frames = (frame_count * average_frames + frames) / (frame_count + 1);
			frame_count += 1;
			update_statistics(frames, updates);
			updates = 0, frames = 0;
#ifdef DEBUG_PROFILING
			temp_profiling_u.push_back(update_time_);
			temp_profiling_s.push_back(render->draw_times[0]);
			temp_profiling_t.push_back(render->draw_times[1]);
			if (frame_count == 10) break;
#endif
		}
	} glfwTerminate();

#ifdef DEBUG_PROFILING
	float average = accumulate(temp_profiling_u.begin(), temp_profiling_u.end(), 0.0) / temp_profiling_u.size();
	cout << "Average update time: " << average * 1000 << endl;
	average = accumulate(temp_profiling_s.begin(), temp_profiling_s.end(), 0.0) / temp_profiling_s.size();
	cout << "Average shapes time: " << average * 1000 << endl;
	average = accumulate(temp_profiling_t.begin(), temp_profiling_t.end(), 0.0) / temp_profiling_t.size();
	cout << "Average text time: " << average * 1000 << endl;
#endif

	log_t("Average FPS after ", frame_count, " seconds: ", average_frames);
	return 0;
}
