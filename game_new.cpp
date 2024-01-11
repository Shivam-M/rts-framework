#include "game_new.h"
// #include "io/keyboard.h"
// #include "io/gamepad.h"
// #include "io/mouse.h"

#include <windows.h>
#include <Psapi.h>

#include "defs/colour.h"
#include "defs/province_new.h"
#include "defs/unit_new.h"
#include "defs/nation_new.h"
#include "defs/object.h"

#define MAX_RESTRICTED_GAME_SPEED 2000
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define PRINT_DEBUG_

using namespace std;

GameNew* GameNew::game = nullptr;
GameNew::GameNew() {}

GameNew::GameNew(int argc, char** argv) {
	double launch_time = glfwGetTime();
	game = this;

	if (!glfwInit()) return;
	if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RTS Game", NULL, NULL))) { glfwTerminate(); return; }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwSetWindowPos(window, 100, 100);
	glfwMakeContextCurrent(window);
	glClearColor(0.155f, 0.155f, 0.155f, 0.f);

	glewExperimental = GL_TRUE;
	glewInit();

	info(ss << "\033[1;31mOpenGL Version: " << glGetString(GL_VERSION));

	render = RenderNew(window, &objects, &text_objects);

	font_data font = Fonts::getFont("data/fonts/sindelar.otf", 11, true);
	font = Fonts::getFont(CONSOLAS_BOLD, 11);

	t_FPSCounter =			TextNew({0.925, 0.050}, font, Colour2(0, 206, 201, 255), "FPS: --");
	t_PlayerLocation =		TextNew({0.030, 0.050}, font, Colour2(34, 166, 179, 255), "--");
	t_PlayerVelocity =		TextNew({0.030, 0.075}, font, Colour2(34, 166, 179, 255), "--");
	t_PlayerAcceleration =	TextNew({0.030, 0.100}, font, Colour2(34, 166, 179, 255), "--");
	t_Alt =					TextNew({0.675, 0.050}, font, Colour2(223, 249, 251, 255), "ALT: --");
	t_Alt2 =				TextNew({0.800, 0.050}, font, Colour2(223, 249, 251, 255), "ALT: --");
	t_Alt3 =				TextNew({0.550, 0.050}, font, Colour2(223, 249, 251, 255), "ALT: --");
	t_Notification =		TextNew({0.150, 0.050}, font, Colour2(189, 195, 199, 255), "");

	int level_counter = 0;
	// registerObject(&player);

	console = new ConsoleNew(this);
	console->build();

	for (Level2 l : levels)
		for (MoveableNew* m : l.objects) {
			m->location.x += level_counter - 1;
			registerObject(m);
		}

	// One-time load
	// Image::loadMap("data/world_map.bmp", "data/province_colours.txt");

	while (true) {
		ifstream ifs("data/levels/rts/" + to_string(level_counter) + ".json");
		if (ifs.fail()) break;
		Level2* level = loader.load("data/levels/rts/" + to_string(level_counter) + ".json", &queue_objects, &text_objects, level_counter);

		for (MoveableNew* m : level->objects) {
			Vector2 location = m->getLocation();
			if (m->getFlags() & QUAD) {
				vector<Vector2> points = m->getPoints();
				CollidableNew* c = (CollidableNew*)(m);
				for (int x = 0; x < 4; x++) points[x].x += level_counter;
				// c->setPoints(points[0], points[1], points[2], points[3]);
			}
			m->location.x += level_counter;
			registerObject(m);
		}

		for (TextNew* t : level->text_objects) registerObject(t);
		levels.push_back(*level);
		level_counter++;
	}

	FILE* neighbours_file;
	fopen_s(&neighbours_file, "data/generated/province_neighbours.txt", "r");
	if (neighbours_file == nullptr) {
		info_e("Error loading province neighbour pairs file.");
		return;
	}

	int id, other_id;
	while (fscanf_s(neighbours_file, "%d,%d", &id, &other_id) == 2) {
		ProvinceNew* province = LoaderNew::getProvinceMap()[id];
		ProvinceNew* other_province = LoaderNew::getProvinceMap()[other_id];
		province->addNeighbour(other_province);
		other_province->addNeighbour(province);
		info(ss << "Assigned province '" << province->getName() << "' (" << province->getID() << ") as a neighbour with '" << other_province->getName() << "' (" << other_province->getID() << ")");
	}
	fclose(neighbours_file);

	FILE* dimensions_file;
	fopen_s(&dimensions_file, "data/generated/province_dimensions.txt", "r");
	if (dimensions_file == nullptr) {
		info_e("Error loading province map data file.");
		return;
	}

	float x, y, w, h;
	float xoffset = -0.2, yoffset = -0.1;
	while (fscanf_s(dimensions_file, "%d,%f,%f,%f,%f", &id, &x, &y, &w, &h) == 5) {
		info_i("Loading map position data for province ID... #" + to_string(id));
		LoaderNew::getProvinceMap()[id]->setSize(w, h);
		LoaderNew::getProvinceMap()[id]->setLocation(x + xoffset, y + yoffset);
		LoaderNew::getProvinceMap()[id]->addFlag(TEXTURED);
	}
	fclose(dimensions_file);

	vector<ProvinceNew*> path;
	// path.push_back(LoaderNew::getProvinceMap()[5]);
	path.push_back(LoaderNew::getProvinceMap()[22]);
	path.push_back(LoaderNew::getProvinceMap()[21]);
	path.push_back(LoaderNew::getProvinceMap()[20]);
	path.push_back(LoaderNew::getProvinceMap()[19]);
	path.push_back(LoaderNew::getProvinceMap()[17]);
	path.push_back(LoaderNew::getProvinceMap()[18]);
	LoaderNew::getUnitMap()[1]->setPath(path);
	LoaderNew::getUnitMap()[1]->initiate();

	for (const auto& p : LoaderNew::getNationMap()) {
		NationNew* nation = p.second;
		nations.push_back(nation);
	}

	player_nation = nations[0];

	registerObject(&t_FPSCounter);
	registerObject(&t_Alt);
	registerObject(&t_Alt2);
	registerObject(&t_Alt3);
	registerObject(&t_Notification);
	registerObject(&t_PlayerVelocity);
	registerObject(&t_PlayerAcceleration);
	registerObject(&t_PlayerLocation);

	for (TextNew* t : text_objects) t->addFlag(BUTTON);

	// keyboard = new Keyboard(this);
	// mouse = new Mouse(this);

	// glfwSetKeyCallback(window, keyboard->callback);
	// glfwSetMouseButtonCallback(window, mouse->callback);
	// glfwSetScrollCallback(window, mouse->scroll_callback);

	info("Took " + to_string(glfwGetTime() - launch_time) + " seconds to load the game.");
}

// Player* GameNew::getPlayer() { return &player; }

void GameNew::checkCollision() {
	/*
	double* location = player.getLocation();
	double* size = player.getSize();
	double x, y;

	for (MoveableNew* m : objects) {
		double* m_location = m->getLocation();
		double* m_size = m->getSize();
		if (m->getFlags() & GHOST) continue;
		else if (m->getFlags() & QUAD) { // STAIRS
			map<double, double> first_point = m->getPoints()[0];
			map<double, double> second_point = m->getPoints()[1];
			if (first_point[0] - size[0] / 2 < location[0] && location[0] < second_point[0] - size[0] / 2) {
				double xdiff = abs(second_point[0] - first_point[0]);
				double ydiff = abs(second_point[1] - first_point[1]);
				double relx = abs((location[0] + size[0] / 2) - first_point[0]) / xdiff;
				double ground = (first_point[1] - (ydiff * relx)) - size[1];
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

void GameNew::fireShot() {
	/*
	Ice* ice = new Ice();
	ice->setLocation(player.getLocation()[0], player.getLocation()[1]);
	ice->setAcceleration(0.0001, 0);
	ice->objects = &queue_objects;
	registerObject(ice);
	*/
}

void GameNew::debugMode() {
	// render.render_level = 2;
}

void GameNew::checkTimers() {
	double current_time = glfwGetTime();
	for (map<double, int> timed_task : timers_);
}

void GameNew::updateStatistics(int f, int u) {
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

	// font_data font = Fonts::getFont("data/fonts/brockscript.ttf", 25, true);
	// t_PlayerAcceleration.setFont(font);
	t_PlayerLocation.setContent("Day #" + to_string(elapsed_days));
	t_PlayerVelocity.setContent("Money: 0.00");
	t_PlayerAcceleration.setContent("Playing as: " + nation_string);
	info(ss << "FPS: " << f << " \tUpdates: " << u << " \tGame time: " << update_time_ << "ms \t[" << (int)(1 / update_time_) << "]");
}

void GameNew::updateProperties() {
	// double x, y, relx, rely;
	int dimensions[2] = { 1280, 720 };
	// glfwGetWindowSize(window, &dimensions[0], &dimensions[1]);
	// glfwGetCursorPos(window, &x, &y);
	// relx = x / dimensions[0], rely = y / dimensions[1];

	if (selected_object) {
		if (selected_object->getFlags() & PROVINCE) {
			selected_object->setColour(Colour2(255, 255, 0));
			ProvinceNew* province = (ProvinceNew*)selected_object;
			for (ProvinceNew* neighbour : province->getNeighbours()) {
				neighbour->setColour(Colour2(255, 0, 255));
			}
		} else if (selected_object->getFlags() & UNIT) {

		}
	}

	if (rbDown && selected_object) {
		double x, y, relx, rely;
		glfwGetCursorPos(window, &x, &y);
		relx = x / dimensions[0], rely = y / dimensions[1];
		Vector2 new_size = abs(game->mouse_position.x - relx), abs(game->mouse_position.y - rely);
		selected_object->setSize(new_size.x, new_size.y);
		t_Notification.setContent("Set size of " + game->selected_object->getName() + " to " + to_string(new_size.x) + ", " + to_string(new_size.y));
	}
	if (mbDown) {
		double x, y, relx, rely;
		glfwGetCursorPos(window, &x, &y);
		relx = x / dimensions[0], rely = y / dimensions[1];
		render.offsets.x += (relx - original_position.x) * 0.01, render.offsets.y += (rely - original_position.y) * 0.01;
	}
}

void GameNew::updateObjects(double modifier) {
	vector<MoveableNew*> inactive_objects;
	for (MoveableNew* m : objects) !m->isActive ? inactive_objects.push_back(m) : m->update(modifier);

	for (MoveableNew* t : inactive_objects) {
		objects.erase(remove(objects.begin(), objects.end(), t), objects.end());
		delete t;
	}

	for (MoveableNew* q : queue_objects) objects.push_back(q);
	queue_objects.clear();
}

void GameNew::registerObject(MoveableNew* m) { objects.push_back(m); }

void GameNew::registerObject(TextNew* t) { text_objects.push_back(t); }

int GameNew::gameLoop() {
	update_rate = 60;
	int frames = 0, updates = 0, frame_count = 0;
	double average_frames = 0;
	double limit = 1.0 / update_rate;
	double last_time = glfwGetTime(), timer = last_time;
	double delta_time = 0, current_time = 0;
	double last_frame_time = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		current_time = glfwGetTime();
		limit = 1.0 / update_rate;
		delta_time += (current_time - last_time) / limit;
		last_time = current_time;

		while (delta_time >= 1.0) {

			double update_time = glfwGetTime();
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
			for (NationNew* nation : nations) nation->evaluate();
#endif
			elapsed_days++;
			updateObjects(60.0 / update_rate);
			// checkCollision();
			updateProperties();
			glfwPollEvents();
			// checkTimers();
			updates++;
			delta_time--;
			update_time_ = glfwGetTime() - update_time;
		}

		if (fpsLimit == 0 || glfwGetTime() - last_frame_time >= (1.0 / fpsLimit)) {
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
		}
	} glfwTerminate();

	// TODO: Memory management for particles
	int counter = 0;
	for (MoveableNew* t : objects) {
		if (t->getFlags() & SQUARE) delete t;
		counter++;
	} objects.clear();

	for (const auto& p : Image::images) {
		Texture* texture = p.second;
		delete[] texture->image;
		delete texture;
	}

	info(ss << "Average FPS after " << frame_count << " seconds: " << average_frames);
	return 0;
}
