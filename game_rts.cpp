#ifdef _WIN32
#include <windows.h>
#include <Psapi.h>
#endif

#include <filesystem>

#include "game_rts.h"
#include "assets/province.h"
#include "assets/unit.h"
#include "assets/nation.h"
#include "io/mouse.h"
#include "io/keyboard.h"
#include "tools/text_renderer.h"


#define MAX_RESTRICTED_GAME_SPEED 2000
// #define DEBUG_PROFILING

using namespace std;
namespace fs = filesystem;

GameRTS* GameRTS::instance = nullptr;

//struct BattleInformation {
//	vector<Unit*> attacker_units; // 1st element always the main attacker
//	vector<Unit*> defender_units; // 1st element always the main defender
//	int total_attacker_starting_strength;
//	int total_defender_starting_strength;
//	int total_attacker_current_strength = 0;
//	int total_defender_current_strength = 0;
//	int defender_losses;
//	Province* province;
//	int get_attacker_strength() {
//		total_attacker_current_strength = 0;
//		for (Unit* unit : attacker_units) {
//			total_attacker_current_strength += unit->getAmount();
//		}
//		return total_attacker_current_strength;
//	}
//	int get_defender_strength() {
//		total_defender_current_strength = 0;
//		for (Unit* unit : defender_units) {
//			total_defender_current_strength += unit->getAmount();
//		}
//		return total_defender_current_strength;
//	}
//	int get_battle_swing() { return total_attacker_current_strength - total_defender_current_strength; }
//};

// TODO: Use new text alignment on unit names and console
// ADD text input field

GameRTS::GameRTS() : Game() { instance = this; }
GameRTS::GameRTS(int a, char** b) : Game(a, b) { instance = this; }

void GameRTS::extendedInitialisation() {
	// Image::loadMap("data/world_map.bmp", "data/province_colours.txt");
#define dbg2
#ifdef dbg
	loadLevels("data/levels/debug/");
#else
	loadLevels("data/levels/rts/");
#endif
	loadLevels("data/levels/ui/");
	loadProvinceNeighbours("data/generated/province_neighbours.txt");
	loadProvinceAttributes("data/generated/province_dimensions.txt");
	setupRTSGame();

	console->build();

	UIManager::Hide("ui_menu_pause");
	UIManager::Hide("ui_nation_tooltip");
	UIManager::Hide("ui_war_declaration");
	UIManager::Hide("ui_event_choice");
	UIManager::Hide("ui_war_indicator");
	UIManager::Hide("ui_unit_hire");
	UIManager::Hide("ui_battle_unit");
	// UIManager::Hide("ui_information_header");

	log_t("Took " CON_RED, glfwGetTime() - launch_time_, " seconds " CON_NORMAL "to completely load the game.");
}

void GameRTS::setupRTSGame() {
	executeAction(CHANGE_MAP_VIEW);
	Loader::getUnitMap()[1]->setPath(Province::getShortestPath(Loader::getProvinceMap()[5], Loader::getProvinceMap()[18]));

	for (const auto& u : Loader::getUnitMap()) {
		Unit* unit = u.second;
		unit->initiate();
	}

	for (const auto& n : Loader::getNationMap()) {
		Nation* nation = n.second;
		nations.push_back(nation);
	}

	player_nation = nations[0];
	for (Moveable* m : objects) if (m->hasFlag(THEMED)) m->setColour(player_nation->getColour().setW(m->getColour().getW()));
}

void loadProvinceNeighbours(string neighbours) {
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
			auto province_map = Loader::getProvinceMap();
			Province* province = province_map[id];
			Province* other_province = province_map[other_id];

			province->addNeighbour(other_province);
			other_province->addNeighbour(province);

			log_t("Assigned province " CON_RED, province->getName(), CON_NORMAL " (" CON_RED, province->getID(), CON_NORMAL ") as a neighbour with " CON_RED, other_province->getName(), CON_NORMAL " (" CON_RED, other_province->getID(), CON_NORMAL ")");
		}
	}
}

void loadProvinceAttributes(string attributes) { // TODO: Change to fstream
	FILE* dimensions_file;
	FOPEN(dimensions_file, attributes.c_str(), "r");
	if (dimensions_file == nullptr) {
		log_t("Error loading province map data file.");
		return;
	}

	int id;
	float x, y, w, h, xoffset = -0.2f, yoffset = -0.1f;
	while (FSCANF(dimensions_file, "%d,%f,%f,%f,%f", &id, &x, &y, &w, &h) == 5) {
		Loader::getProvinceMap()[id]->setSize(w, h);
		Loader::getProvinceMap()[id]->setLocation(x + xoffset, y + yoffset);
	}
	log_t("Loaded map position data for all provinces");
	fclose(dimensions_file);
}

void GameRTS::pauseGame() {
	game_paused = !game_paused;
	simulation_paused = !simulation_paused;
	UIManager::Toggle("ui_menu_pause", true);
}

void GameRTS::incrementDay() {
	if (++date.day > month_days[date.month]) {
		date.day = 1;

		if (++date.month > 12) {
			date.month = 1;
			date.year++;
		}
	}
}

string GameRTS::getDate() {
	string day_suffix;
	switch (date.day % 10) {
		case 1: day_suffix = "st"; break;
		case 2: day_suffix = "nd"; break;
		case 3: day_suffix = "rd"; break;
		default: day_suffix = "th"; break;
	} return to_string(date.day) + day_suffix + " " + month_names[date.month - 1] + " " + to_string(date.year) + " AD";
}

void GameRTS::updateStatistics(int f, int u) {
	Game::updateStatistics(f, u);

	string nation_string = "-- [X]";
	ostringstream nation_treasury;
	if (player_nation) {
		nation_string = player_nation->getName() + " [" + to_string(player_nation->getID()) + "]";

		const float& money = player_nation->getMoney();
		t_Information2.setContent(format("Money: {:.2f}", money));
		t_Information3.setColour(player_nation->getColourRef());
	}

	t_Information.setContent(getDate());
	t_Information3.setContent("Playing as: " + nation_string);
}

static bool within(const Vector2& location, const Vector2& size, const Vector2& point) {
	return point.x > location.x && point.x < location.x + size.x && point.y > location.y && point.y < location.y + size.y;
}

void GameRTS::expandNation(Province* province) {
	viewed_nation = province->getNation();
	UIManager::Show("ui_nation_tooltip");
}

void GameRTS::moveUnit(Province* province) {
	Unit* unit = reinterpret_cast<Unit*>(selected_object);
	if (unit->getState() != Unit::FIGHTING && unit->getState() != Unit::DEAD ) {
		unit->setPath(Province::getShortestPath(unit->getProvince(), province));
		unit->initiate();
	}
}

Moveable* GameRTS::getObjectUnderMouse() {
	Moveable* object = nullptr;
	float min_distance = 100.f;
	for (Moveable* moveable : game->objects) {
		if (moveable->getFlags() & (UNEDITABLE | DISABLED)) continue;
		if (!game_paused) moveable->onHoverStop();

		Vector2 location = moveable->getLocation() * render.scale + render.offsets;
		Vector2 size = moveable->getSize() * render.scale;

		if (!within(location, size, cursor_position)) continue;

		if (moveable->hasFlag(UI)) {
			object = moveable;
			min_distance = -1;
			continue;
		}

		Vector2 centre = moveable->getCentre() * render.scale + render.offsets;
		
		float distance = (centre.x - cursor_position.x) * (centre.x - cursor_position.x) + (centre.y - cursor_position.y) * (centre.y - cursor_position.y);
		if (distance < min_distance) {
			min_distance = distance;
			object = moveable;
		}
	}

	UIManager::Hide("ui_province_tooltip");
	UIManager::Hide("ui_unit_tooltip");

	if (object && !object->hasFlag(DISABLED)) {
		if (object->hasFlag(BUTTON)) {
			object->onHover();
		}

		if (object->hasFlag(PROVINCE)) {
			Province* province = reinterpret_cast<Province*>(object);

			if (!picking_nation) {
				object->onHover();
				hoverProvince(province);
			} else {
				object->onHoverStop();
				for (Province* province : province->getNation()->getOwnedProvinces()) province->onHover();
			}
		} else if (object->hasFlag(UNIT)) {
			hoverUnit(reinterpret_cast<Unit*>(object));
		}
	}
	
	hovered_object = object;
	return object;
}

void GameRTS::hoverProvince(Province* province) {
	UIManager::AssignValues("ui_province_tooltip", province);
	UIManager::Show("ui_province_tooltip");
}

void GameRTS::hoverUnit(Unit* unit) {
	UIManager::AssignValues("ui_unit_tooltip", unit);
	UIManager::Show("ui_unit_tooltip");
}

void GameRTS::updateCursor() {
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	cursor_position.set(static_cast<float>(x / render.resolution.x), static_cast<float>(y / render.resolution.y));
}

void GameRTS::executeAction(BUTTON_ACTION action, Moveable* button) {  // keep option for action-only
	log_t("Executing button action: " CON_RED, action);
	static int index;
	static string files[3] = { "data/images/map.png", "data/images/map_white.png", "data/images/map_blue.png" };

	static int index_font;
	static vector<string> files_font;
	Unit* hired_unit;
	War war;

	// Panel* panel = UIManager::GetPanel("ui_information_header");

	switch (action) {
		case CHANGE_CONTROLS:
			game->mouse->debug_control_scheme ^= true;
			t_Notification.setContent(format("Set debug control mode to: {}", game->mouse->debug_control_scheme));
			break;
		case PAUSE_GAME:
			pauseGame();
			break;
		case PAUSE_SIMULATION:
			simulation_paused = !simulation_paused;
			break;
		case SWITCH_NATION:
			picking_nation ^= true;
			t_Notification.setContent("Select a nation to control it");
			break;
		case CHANGE_MAP_VIEW: // Change to a mask
			value_view ^= true;
			// queue action to be executed in getObjectUnderMouse() instead of cycling through moveables again?
			for (Moveable* moveable : objects) {
				if (moveable->hasFlag(PROVINCE)) {
					Colour colour = moveable->getColour();
					colour.setW(!value_view ? 200 : 75 + (reinterpret_cast<Province*>(moveable)->getValue()) * 200);
					moveable->setColour(colour);
				}
			}
			break;
		case SAVE_GAME:
			Loader::writeObjects(objects, text_objects);
			if (index > 2) index = 0;
			objects[0]->setTexture(Image::getImage(files[index]));
			index += 1;
			break;
		case HIRE_UNIT:
			player_nation->setMoney(999999999.f);
			hired_unit = player_nation->hireUnit(999, 5.f);
			if (hired_unit) {
				Font* font = Fonts::getFont("data/fonts/Cinzel-Bold.ttf", 16, true); // (189, 195, 199, 250)
				Text* unit_text = new Text(hired_unit->getLocation(), font, Colour(220, 221, 225, 200), hired_unit->getName(), 0.5f);
				unit_text->addFlag(TEXT_BACKGROUND | UNSAVEABLE);
				unit_text->removeFlag(FIXED_POS);
				hired_unit->setName(player_nation->getName() + " Hired Unit");
				hired_unit->setSize(0.02125f, 0.0375f);
				hired_unit->setText(unit_text);
				hired_unit->setTextOffset(0.f, -0.0025f);
				hired_unit->initiate();
				registerObject(hired_unit);
				registerObject(unit_text);
			}
			break;
		case TEST_FONTS:
			if (files_font.size() == 0) {
				string path = ".\\data\\fonts";
				for (const auto& file : fs::directory_iterator(path))
					files_font.push_back(file.path().string());
			}

			if (index_font > files_font.size() - 1) index_font = 0;
			for (const auto& u : Loader::getUnitMap()) {
				Unit* unit = u.second;
				unit->getText()->setFont(Fonts::getFont(files_font[index_font], 16, true));
			}
			
			/*for (Moveable* panel_item : *panel->get()) {
				if (panel_item->getName() == "ui_information_header_date") {
					Text* panel_text = reinterpret_cast<Text*>(panel_item);
					panel_text->setFont(Fonts::getFont(files_font[index_font], panel_text->getFont()->h, true));
				}
				if (panel_item->getName() == "ui_information_header_gold") {
					Text* panel_text = reinterpret_cast<Text*>(panel_item);
					panel_text->setFont(Fonts::getFont(files_font[index_font], panel_text->getFont()->h, true));
				}
			}*/

			t_Notification.setContent("Set game font to " + files_font[index_font]);
			index_font++;
			break;
		case TOGGLE_TOOLTIP:
			UIManager::Toggle("ui_nation_tooltip");
			break;
		case UI_DEBUG_TOGGLE:
			UIManager::Toggle("ui_war_indicator");
			UIManager::Toggle("ui_war_declaration");
			UIManager::Toggle("ui_event_choice");
			UIManager::Toggle("ui_unit_hire");
			UIManager::Toggle("ui_information_header");
			break;
		case DECLARE_WAR:
			war.attacker = player_nation;
			war.defender = viewed_nation;
			war.war_goal = TAKE_KEY_PROVINCE;
			war.war_goal_target.target_province = viewed_nation->getCapital();
			/*war.attacker_allies.push_back(nations[1]);
			war.attacker_allies.push_back(nations[3]);
			war.defender_allies.push_back(nations[4]);
			war.defender_allies.push_back(nations[5]);
			war.defender_allies.push_back(nations[6]);
			war.defender_allies.push_back(nations[9]);*/
			UIManager::AssignValues("ui_war_declaration", &war);
			UIManager::Hide("ui_nation_tooltip");
			UIManager::Show("ui_war_declaration");
			break;
		default:
			t_Notification.setContent(format("Unsupported button action: {}", (int)action));
	}
}

static BattleInformation* some_battle_info = nullptr;
void GameRTS::registerEvent(Event event, void* details) {
	switch (event) {
		case START_BATTLE:
			some_battle_info = static_cast<BattleInformation*>(details);
			
			break;
		default:
			log_t("Unsupported event: " CON_RED, event);
			break;
	}
}

void GameRTS::updateProperties() {
	updateCursor();
	getObjectUnderMouse();

	if (viewed_nation)
		UIManager::AssignValues("ui_nation_tooltip", viewed_nation);
	
	if (some_battle_info)
		UIManager::AssignValues("ui_battle_unit", some_battle_info);

	if (getButton(GLFW_MOUSE_BUTTON_MIDDLE)) render.offsets.x += (cursor_position.x - original_position.x) * 0.01f, render.offsets.y += (cursor_position.y - original_position.y) * 0.01f;

	if (!selected_object) return;

	if (holding_left_mouse_button) {
		Moveable* draggable_panel = selected_object;

		while (draggable_panel && draggable_panel->parent && !(draggable_panel->getFlags() & DRAGGABLE)) {
			draggable_panel = draggable_panel->parent;
		}

		if (draggable_panel and draggable_panel->hasFlag(DRAGGABLE)) {
			if (draggable_panel != dragged_object) {
				drag_offset = cursor_position - draggable_panel->location;
				dragged_object = draggable_panel;
			}
			draggable_panel->setLocation((cursor_position - drag_offset).x, (cursor_position - drag_offset).y);
		} else {
			dragged_object = nullptr;
		}
	} else {
		dragged_object = nullptr;
	}

	if (selected_object->hasFlag(PROVINCE) && picking_nation) {
		player_nation = reinterpret_cast<Province*>(selected_object)->getNation();
		picking_nation = false;

		for (Moveable* m : objects) {
			if (m->hasFlag(THEMED)) {
				m->setColour(player_nation->getColour().setW(m->getColourRef().getW()));
			}
		}

		log_t("Player is now controlling nation " CON_RED, player_nation->getName(), CON_NORMAL " (" CON_RED, player_nation->getID(), CON_NORMAL ")");
	}

	if (mouse->debug_control_scheme) {
		if (selected_object->hasFlag(PROVINCE)) {
			selected_object->setColour(Colour(255, 255, 0, 80));
			for (Province* neighbour : reinterpret_cast<Province*>(selected_object)->getNeighbours()) {
				neighbour->setColour(Colour(255, 0, 255, 80));
			}
		}
		if (getButton(GLFW_MOUSE_BUTTON_RIGHT)) {
			Vector2 new_size = Vector2(abs(game->mouse_position.x - cursor_position.x), abs(game->mouse_position.y - cursor_position.y));
			selected_object->setSize(new_size.x, new_size.y);
			t_Notification.setContent("Set size of " + game->selected_object->getName() + " to " + to_string(new_size.x) + ", " + to_string(new_size.y));
		}
	}

	if (selected_object->hasFlag(BUTTON)) {
		executeAction(selected_object->getButtonAction(), selected_object);
		selected_object = nullptr;
	}
}


void GameRTS::updateObjects(float modifier) {
	// sort(objects.begin(), objects.end(), [](Moveable* a, Moveable* b) {
	//	 return a->getPriority() < b->getPriority();
	// }); // Temp, use z values for priorities instead in a Vector3

	vector<Moveable*> inactive_objects;
	for (Moveable* m : objects) !m->is_active ? inactive_objects.push_back(m) : m->update(modifier);
	for (Moveable* to : text_objects) !to->is_active ? inactive_objects.push_back(to) : to->update(modifier);

	for (Moveable* t : inactive_objects) {
		objects.erase(remove(objects.begin(), objects.end(), t), objects.end());
		delete t;
	}

	HeaderInformation header_information = { player_nation->getMoney(), getDate()};

	if (!UIManager::GetPanel("ui_information_header")->hasFlag(DISABLED))
		UIManager::AssignValues("ui_information_header", &header_information);

	for (Moveable* q : queue_objects) objects.push_back(q);
	queue_objects.clear();
}

int GameRTS::gameLoop() {
	int frames = 0, updates = 0, frame_count = 0;
	float average_frames = 0.f;
	float limit = 1.0f / update_rate;
	float last_time = static_cast<float>(glfwGetTime()), timer = last_time;
	float delta_time = 0.f, current_time = 0.f;
	float last_frame_time = static_cast<float>(glfwGetTime());
	float update_time;

#ifdef DEBUG_PROFILING
	vector<float> temp_profiling_u;
	vector<float> temp_profiling_t;
	vector<float> temp_profiling_s;
	fps_limit = 0;
#endif

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		current_time = glfwGetTime();
		limit = 1.0f / update_rate;
		delta_time += (current_time - last_time) / limit;
		last_time = current_time;

		while (delta_time >= 1.0f) {
			update_time = glfwGetTime();
			if (!game_paused && !simulation_paused) {
				//#pragma omp parallel for num_threads(12)
				for (Nation* nation : nations) nation->evaluate();
				incrementDay();
			}
			updateObjects(60.0f / update_rate);
			updateProperties();
			updates++;
			delta_time--;
			update_time_ = static_cast<float>(glfwGetTime()) - update_time;
		}

		if (fps_limit == 0 || glfwGetTime() - last_frame_time >= (1.0f / fps_limit)) {
			last_frame_time = static_cast<float>(glfwGetTime());
			render.renderWindow();
			frames++;
		}

		if (glfwGetTime() - timer > 1.0f) {
			timer++;
			average_frames = (frame_count * average_frames + frames) / (frame_count + 1);
			frame_count += 1;
			updateStatistics(frames, updates);
			updates = 0, frames = 0;
#ifdef DEBUG_PROFILING
			temp_profiling_u.push_back(update_time_);
			temp_profiling_s.push_back(render.draw_times[0]);
			temp_profiling_t.push_back(render.draw_times[1]);
			if (frame_count == 10) break;
#endif
		}
	} glfwTerminate();

	for (const auto& p : Image::images) {
		delete[] p.second->image;
		delete p.second;
	}

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
