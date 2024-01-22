#include <windows.h>
#include <Psapi.h>

#include "game.h"
#include "defs/province.h"
#include "defs/unit.h"
#include "defs/nation.h"
#include "io/mouse.h"
#include "io/keyboard.h"
#include "assets/general_tooltip.h"
#include "assets/particle_group.h"

#include "tools/text_renderer.h"

#define MAX_RESTRICTED_GAME_SPEED 2000
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define PRINT_DEBUG_
// #define DEBUG_PROFILING

#define GAME_RTS rts
#define GAME_SIDESCROLLER sidescroller
#define GAME_TYPE GAME_RTS

using namespace std;

Game* Game::game = nullptr;

Game::Game(int argc, char** argv) {
	float launch_time = glfwGetTime();
	game = this;

	if (!glfwInit()) return;
	if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RTS Game", NULL, NULL))) { glfwTerminate(); return; }

	glfwSetWindowPos(window, 100, 100);
	glfwMakeContextCurrent(window);
	glClearColor(0.155f, 0.155f, 0.155f, 0.f);

	glewExperimental = GL_TRUE;
	glewInit();
	srand(time(nullptr));

	log_t("\033[1;31mOpenGL Version: ", glGetString(GL_VERSION));

	render = Render(window, &objects, &text_objects);
	keyboard = new Keyboard(this);
	mouse = new Mouse(this);
	console = new Console(this);

	Font* debug_font = Fonts::getFont(CONSOLAS_BOLD, 11);
	t_FPSCounter =			Text({0.925, 0.050}, debug_font, Colour(0, 206, 201, 255), "FPS: --");
	t_PlayerLocation =		Text({0.030, 0.050}, debug_font, Colour(34, 166, 179, 255), "--");
	t_PlayerVelocity =		Text({0.030, 0.075}, debug_font, Colour(34, 166, 179, 255), "--");
	t_PlayerAcceleration =	Text({0.030, 0.100}, debug_font, Colour(34, 166, 179, 255), "--");
	t_Alt =					Text({0.675, 0.050}, debug_font, Colour(223, 249, 251, 255), "ALT: --");
	t_Alt2 =				Text({0.800, 0.050}, debug_font, Colour(223, 249, 251, 255), "ALT: --");
	t_Alt3 =				Text({0.550, 0.050}, debug_font, Colour(223, 249, 251, 255), "ALT: --");
	t_Notification =		Text({0.150, 0.050}, debug_font, Colour(189, 195, 199, 255), "");

	Moveable star = Moveable({}, { 0.005 * 9 / 16.0, 0.005 }, Colour(255, 255, 255, 255), Colour(255, 255, 255, 255));
	ColourShift colourshift = ColourShift(star.getColour(), Colour(255, 255, 255, 0));
	colourshift.speed = 0.02f;
	colourshift.with_gradient = true;
	colourshift.fade_to_death = true;
	star.setColourShift(colourshift);

	// ParticleGroup* stars = new ParticleGroup({ 0.0, 0.0 }, { 1.0, 1.0 }, star, 50, &queue_objects);
	// registerObject(stars);

	// Image::loadMap("data/world_map.bmp", "data/province_colours.txt");
	loadLevels("data/levels/rts/");
	// mouse->debug_control_scheme = true;
	loadProvinceNeighbours("data/generated/province_neighbours.txt");
	loadProvinceAttributes("data/generated/province_dimensions.txt");
	setupRTSGame();
	
	registerObject(&t_FPSCounter);
	registerObject(&t_Alt);
	registerObject(&t_Alt2);
	registerObject(&t_Alt3);
	registerObject(&t_Notification);
	registerObject(&t_PlayerVelocity);
	registerObject(&t_PlayerAcceleration);
	registerObject(&t_PlayerLocation);

	province_tooltip = new GeneralTooltip();
	province_tooltip->setLocation(0.01, 0.79);
	province_tooltip->setTitle("");
	registerObject(province_tooltip);
	
	console->build();

	glfwSetKeyCallback(window, keyboard->callback);
	glfwSetMouseButtonCallback(window, mouse->callback);
	glfwSetScrollCallback(window, mouse->scroll_callback);

	log_t("Took " CON_RED, glfwGetTime() - launch_time,  " seconds " CON_NORMAL "to load the game.");
}

void Game::loadLevels(string level_directory) {
	int level_counter = 0;
	while (true) {
		if (ifstream(level_directory + to_string(level_counter) + ".json").fail()) break;
		Level* level = loader.load_font(level_directory + to_string(level_counter) + ".json", &queue_objects, &text_objects, level_counter);

		for (Moveable* m : level->objects) {
			m->location.x += level_counter;
			registerObject(m);
		}

		for (Text* t : level->text_objects) registerObject(t);
		levels.push_back(*level);
		level_counter++;
	}
}

void Game::setupRTSGame() {
	Loader::getUnitMap()[1]->setPath(Province::getShortestPath(Loader::getProvinceMap()[5], Loader::getProvinceMap()[18]));
	Loader::getUnitMap()[1]->initiate();
	Loader::getUnitMap()[2]->initiate();

	for (const auto& p : Loader::getNationMap()) {
		Nation* nation = p.second;
		nations.push_back(nation);
	}

	player_nation = nations[0];
}

void loadProvinceNeighbours(string neighbours) {
	FILE* neighbours_file;
	fopen_s(&neighbours_file, neighbours.c_str(), "r");
	if (neighbours_file == nullptr) {
		log_t(CON_RED "Error loading province neighbour pairs file." CON_NORMAL);
		return;
	}

	int id, other_id;
	while (fscanf_s(neighbours_file, "%d,%d", &id, &other_id) == 2) {
		Province* province = Loader::getProvinceMap()[id];
		Province* other_province = Loader::getProvinceMap()[other_id];
		province->addNeighbour(other_province);
		other_province->addNeighbour(province);
		log_t("Assigned province " CON_RED, province->getName(), CON_NORMAL " (" CON_RED, province->getID(), CON_NORMAL ") as a neighbour with " CON_RED, other_province->getName(), CON_NORMAL " (" CON_RED, other_province->getID(), CON_NORMAL ")");
	}
	fclose(neighbours_file);
}

void loadProvinceAttributes(string attributes) {
	FILE* dimensions_file;
	fopen_s(&dimensions_file, attributes.c_str(), "r");
	if (dimensions_file == nullptr) {
		log_t("Error loading province map data file.");
		return;
	}

	int id;
	float x, y, w, h, xoffset = -0.2, yoffset = -0.1;
	while (fscanf_s(dimensions_file, "%d,%f,%f,%f,%f", &id, &x, &y, &w, &h) == 5) {
		Loader::getProvinceMap()[id]->setSize(w, h);
		Loader::getProvinceMap()[id]->setLocation(x + xoffset, y + yoffset);
		// Loader::getProvinceMap()[id]->setColour(Colour(0, 0, 0, 80));
		Colour colour = Loader::getProvinceMap()[id]->getColour();
		// colour = colour / 1.25;
		colour.setW(200);
		Loader::getProvinceMap()[id]->setColour(colour);

		if (id == 5) {
			ColourShift colourshift = ColourShift(Loader::getProvinceMap()[id]->getColour(), Loader::getProvinceMap()[10]->getColour());
			colourshift.speed = 0.035f;
			Loader::getProvinceMap()[5]->setColourShift(colourshift);
		}

		// ColourShift colourshift = ColourShift(Loader::getProvinceMap()[id]->getColour(), Colour(20, 20, 20, 50));
		// colourshift.speed = 0.01f;
		// Loader::getProvinceMap()[id]->setColourShift(colourshift);
		// Loader::getProvinceMap()[id]->loadScript("data/scripts/anim.txt");
	}
	log_t("Loaded map position data for all provinces");
	fclose(dimensions_file);
}

void Game::pauseGame() {
	Colour col = objects[0]->getColour();
	col.setW(200);

	Text* t_Start = new Text({ 0.05, 0.35 }, Fonts::getFont("data/fonts/blkchcry.ttf", 50, true), col, "Resume");
	registerObject(t_Start);

	Text* t_Reload = new Text({ 0.05, 0.425 }, Fonts::getFont("data/fonts/blkchcry.ttf", 30, true), col, "Reload scenario");
	registerObject(t_Reload);

	Text* t_Quit = new Text({ 0.05, 0.49 }, Fonts::getFont("data/fonts/blkchcry.ttf", 30, true), col, "Quit game");
	registerObject(t_Quit);
}

void Game::checkCollision() {
	/*
	float* location = player.getLocation();
	float* size = player.getSize();
	float x, y;

	for (Moveable* m : objects) {
		float* m_location = m->getLocation();
		float* m_size = m->getSize();
		if (m->getFlags() & GHOST) continue;
		else if (m->getFlags() & QUAD) { // STAIRS
			map<float, float> first_point = m->getPoints()[0];
			map<float, float> second_point = m->getPoints()[1];
			if (first_point[0] - size[0] / 2 < location[0] && location[0] < second_point[0] - size[0] / 2) {
				float xdiff = abs(second_point[0] - first_point[0]);
				float ydiff = abs(second_point[1] - first_point[1]);
				float relx = abs((location[0] + size[0] / 2) - first_point[0]) / xdiff;
				float ground = (first_point[1] - (ydiff * relx)) - size[1];
				if (location[1] >= ground) {
					player.setLocation(location[0] + m->getVelocity()[0], ground + m->getVelocity()[1]);
					player.setVelocity(player.getVelocity()[0], 0);
					player.removeFlag(PHYSICS);
					return;
				}
			}
		}
		else if (m->getFlags() & COLLIDABLE) {
			if (m_location[0] - size[0] / 2 < location[0] && location[0] < m_location[0] + m_size[0] - size[0] / 2)
				if (location[1] + size[1] >= m_location[1] && location[1] + size[1] <= m_location[1] + 0.005) {
					player.setLocation(location[0] + m->getVelocity()[0], m_location[1] - size[1]);
					player.setVelocity(player.getVelocity()[0], 0);
					player.removeFlag(PHYSICS);
					return;
				}
		}
	}
	if (player.getLocation()[1] >= 1.0 && !god_mode) {
		player.setLocation(0.05, 0.3);
		lives--;
	}
	player.addFlag(PHYSICS);
	player.setVelocity(player.getVelocity()[0], GRAVITY);
	*/
}

void Game::fireShot() {
	/*
	Ice* ice = new Ice();
	ice->setLocation(player.getLocation()[0], player.getLocation()[1]);
	ice->setAcceleration(0.0001, 0);
	ice->objects = &queue_objects;
	registerObject(ice);
	*/
}

void Game::debugMode() { render.setRenderLevel(3); }

void Game::toggleDebug() {
	mouse->debug_control_scheme ^= true;
	t_Notification.setContent("Updated control scheme to: " + to_string(mouse->debug_control_scheme));
}

void Game::updateStatistics(int f, int u) {
	if (console->visible()) {
		PROCESS_MEMORY_COUNTERS memCounter;
		BOOL result = K32GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));
		console->update("Objects: " + to_string(objects.size()) + "    Update Rate: " + to_string(update_rate) + "    Memory: " + to_string(memCounter.WorkingSetSize / 1048576) + "MB");
	}

	string t_time = to_string(render.draw_times[1] * 1000), s_time = to_string(render.draw_times[0] * 1000), u_time = to_string(update_time_ * 1000);
	t_FPSCounter.setContent("FPS: " + to_string(f));
	t_Alt.setContent("T: " + t_time.substr(0, t_time.size() - 2) + "ms");
	t_Alt2.setContent("S: " + s_time.substr(0, s_time.size() - 2) + "ms");
	t_Alt3.setContent("U: " + u_time.substr(0, u_time.size() - 2) + "ms");

	string nation_string = "-- [X]";
	if (player_nation) {
		nation_string = player_nation->getName() + " [" + to_string(player_nation->getID()) + "]";
		t_PlayerAcceleration.setColour(player_nation->getColour());
	}

	t_PlayerLocation.setContent("Day #" + to_string(elapsed_days));
	t_PlayerVelocity.setContent("Money: 0.00");
	t_PlayerAcceleration.setContent("Playing as: " + nation_string);
	log_t("FPS: ", f, " \tUpdates: ", u, " \tGame time: ", update_time_, "s \t[", (int)(1 / update_time_), "]");
}

static bool within(Vector2 location, Vector2 size, Vector2 point) {
	return point.x > location.x && point.x < location.x + size.x && point.y > location.y && point.y < location.y + size.y;
}

void Game::moveUnit(Province* province) {
	Unit* unit = reinterpret_cast<Unit*>(selected_object);
	if (unit->getState() != FIGHTING) {
		unit->setPath(Province::getShortestPath(unit->getProvince(), province));
		unit->initiate();
	}
}

Moveable* Game::getObjectUnderMouse() { // Cache each update
	vector<Moveable*> over_objects;
	Moveable* object = nullptr;
	float min_distance = 100;
	for (Moveable* moveable : game->objects) {
		moveable->onHoverStop();
		if (!(moveable->getFlags() & UNEDITABLE || moveable->getFlags() & FIXED_POS)) {
			Vector2 location = moveable->getLocation() * render.scale + render.offsets, size = moveable->getSize() * render.scale;
			if (within(location, size, cursor_position)) {
				Vector2 centre = moveable->getCentre() * render.scale + render.offsets;
				float distance = pow(centre.x - cursor_position.x, 2) + pow(centre.y - cursor_position.y, 2);
				if (distance < min_distance) {
					min_distance = distance;
					object = moveable;
				}
			}
		}
	}

	if (object != nullptr) {
		if (!object->hasFlag(DISABLED)) {
			object->onHover();
			if (object->hasFlag(PROVINCE)) {
				hoverProvince((Province*)object);
			} else if (object->hasFlag(UNIT)) {
				hoverUnit((Unit*)object);
			}
		}
	} else {
		province_tooltip->hide();
	}
	
	hovered_object = object;
	return object;
}

void Game::hoverProvince(Province* province) {
	province_tooltip->show();
	province_tooltip->setTitle(province->getName());
	province_tooltip->setSubtitle("Owned by: " + province->getNation()->getName());
	province_tooltip->subtitle2->setContent("Value: " + to_string(province->getValue()));
	province_tooltip->subtitle3->setContent("Terrain: " + province->getTerrain());
}

void Game::hoverUnit(Unit* unit) {
	province_tooltip->show();
	province_tooltip->setTitle(unit->getName());
	province_tooltip->setSubtitle("Owned by: " + unit->getNation()->getName());
	province_tooltip->subtitle2->setContent("Skill rating: " + to_string(unit->getSkill()));
	province_tooltip->subtitle3->setContent("Size: " + to_string(unit->getAmount()));
}

void Game::updateCursor() {
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	cursor_position.set(x / render.resolution.x, y / render.resolution.y);
}

void Game::updateProperties() {
	updateCursor();
	getObjectUnderMouse();

	if (mouse->debug_control_scheme) {
		if (selected_object) {
			if (selected_object->getFlags() & PROVINCE) {
				selected_object->setColour(Colour(255, 255, 0, 80));
				Province* province = reinterpret_cast<Province*>(selected_object);
				for (Province* neighbour : province->getNeighbours()) {
					neighbour->setColour(Colour(255, 0, 255, 80));
				}
			}
		}
		if (getButton(GLFW_MOUSE_BUTTON_RIGHT) && selected_object) {
			Vector2 new_size = Vector2(abs(game->mouse_position.x - cursor_position.x), abs(game->mouse_position.y - cursor_position.y));
			selected_object->setSize(new_size.x, new_size.y);
			t_Notification.setContent("Set size of " + game->selected_object->getName() + " to " + to_string(new_size.x) + ", " + to_string(new_size.y));
		}
	}
	if (getButton(GLFW_MOUSE_BUTTON_MIDDLE)) {
		render.offsets.x += (cursor_position.x - original_position.x) * 0.01, render.offsets.y += (cursor_position.y - original_position.y) * 0.01;
	}
}

void Game::updateObjects(float modifier) {
	vector<Moveable*> inactive_objects;
	for (Moveable* m : objects) !m->is_active ? inactive_objects.push_back(m) : m->update(modifier);
	for (Moveable* to : text_objects) !to->is_active ? inactive_objects.push_back(to) : to->update(modifier);

	for (Moveable* t : inactive_objects) {
		objects.erase(remove(objects.begin(), objects.end(), t), objects.end());
		delete t;
	}

	for (Moveable* q : queue_objects) objects.push_back(q);
	queue_objects.clear();
}

void Game::registerObject(Moveable* m) { 
	objects.push_back(m);
	if (m->getFlags() & PANEL) {
		Panel* panel = reinterpret_cast<Panel*>(m);
		for (Moveable* moveable : *panel->get()) {
			if (moveable->getFlags() & TEXT)
				registerObject(reinterpret_cast<Text*>(moveable));
			else
				registerObject(moveable);
		}
	}
}

void Game::registerObject(Text* t) { text_objects.push_back(t); }

int Game::gameLoop() {
	int frames = 0, updates = 0, frame_count = 0;
	float average_frames = 0.f;
	float limit = 1.0f / update_rate;
	float last_time = glfwGetTime(), timer = last_time;
	float delta_time = 0.f, current_time = 0.f;
	float last_frame_time = glfwGetTime();
	float update_time;

#ifdef DEBUG_PROFILING
	vector<float> temp_profiling_u;
	vector<float> temp_profiling_t;
	vector<float> temp_profiling_s;
#endif

	while (!glfwWindowShouldClose(window)) {
		current_time = glfwGetTime();
		limit = 1.0 / update_rate;
		delta_time += (current_time - last_time) / limit;
		last_time = current_time;

		while (delta_time >= 1.0) {
			update_time = glfwGetTime();
#ifdef PRINT_DEBUG
			cout << "* Day #" << elapsed_days << endl;
			for (Nation* nation : nations) {
				nation->evaluate();
				cout << nation->getName() << "\t" << nation->getCapital()->getName() << " (" << nation->getNumberProvinces() << ")\tMoney: " << nation->getMoney() << " [" << nation->getIncome() << "]\t";
				for (ProvinceMV* province : nation->getOwnedProvinces()) {
					cout << province->getName() << ", ";
				} cout << endl;
				for (Unit* unit : nation->getOwnedUnits()) {
					string target_name = unit->getTarget() ? unit->getTarget()->getName() : "None";
					cout << "Unit #" << unit->getID() << "\tSize: " << unit->getSize() << "\t Location: " << unit->getX() << ", " << unit->getY() << "\tProvince: " << unit->getProvince()->getName() << "\t\tTarget: " << target_name << endl;
				} cout << endl;
			} cout << endl;
#else
			for (Nation* nation : nations) nation->evaluate();
#endif
			elapsed_days++;
			updateObjects(60.0 / update_rate);
			// checkCollision();
			updateProperties();
			glfwPollEvents();
			updates++;
			delta_time--;
			update_time_ = glfwGetTime() - update_time;
		}

		if (fps_limit == 0 || glfwGetTime() - last_frame_time >= (1.0 / fps_limit)) {
			last_frame_time = glfwGetTime();
			render.renderWindow();
			frames++;
		}

		if (glfwGetTime() - timer > 1.0) {
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

	// TODO: Memory management for particles
	int counter = 0;
	for (Moveable* t : objects) {
		if (t->getFlags() & SQUARE) delete t;
		counter++;
	} objects.clear();

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
