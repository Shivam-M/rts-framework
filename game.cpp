


#include "game.h"
#include "io/keyboard.h"
#include "io/gamepad.h"
#include "io/mouse.h"

#include <windows.h>
#include <Psapi.h>

#include "defs/colour.h"
#include "defs/province.h"
#include "defs/province_mv.h"
#include "defs/unit.h"
#include "defs/nation.h"
#include "defs/object.h"

#define MAX_RESTRICTED_GAME_SPEED 2000
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define PRINT_DEBUG_

vector<Nation*> nations;
GLFWwindow* window;

Unit* selected_unit; ProvinceMV* selected_province;
double game_speed = 2.5;
int elapsed_days = 0;

Nation* player_nation = nullptr;

using namespace std;

Game* Game::game = nullptr;
Game::Game() {}

Game::Game(int argc, char** argv) {
	double launch_time = glfwGetTime();
	stringstream ss;
	game = this;

	Moveable::setGameInstance(this);

	// debug_log("This", "Is", "A ", 2.2, "", 1, " test");

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

	render = Render(window, &objects, &text_objects, &secondary_objects);

	font_data font = Fonts::getFont("data/fonts/sindelar.otf", 11, true);
	font = Fonts::getFont(CONSOLAS_BOLD, 11);

	t_FPSCounter =			Text(0.925, 0.050, font,   0, 206, 201, 255, "FPS: --");
	t_PlayerLocation =		Text(0.030, 0.050, font,  34, 166, 179, 255, "--");
	t_PlayerVelocity =		Text(0.030, 0.075, font,  34, 166, 179, 255, "--");
	t_PlayerAcceleration =	Text(0.030, 0.100, font,  34, 166, 179, 255, "--");
	t_Alt =					Text(0.675, 0.050, font, 223, 249, 251, 255, "ALT: --");
	t_Alt2 =				Text(0.800, 0.050, font, 223, 249, 251, 255, "ALT: --");
	t_Alt3 =				Text(0.550, 0.050, font, 223, 249, 251, 255, "ALT: --");
	t_Notification =		Text(0.150, 0.050, font, 189, 195, 199, 255, "");

	int level_counter = 0;
	// registerObject(&player);

	console = new Console(this);
	console->build();

	if (LEVEL_TRANSITION_MODE)
	for (Level l : levels)
	for (Moveable* m : l.objects) {
		double* location = m->getLocation();
		m->setLocation(location[0] + level_counter - 1, location[1]);
		registerObject(m);
	}

	// One-time load
	// Image::loadMap("data/world_map.bmp", "data/province_colours.txt");

	while (true) {
		ifstream ifs("data/levels/rts/" + to_string(level_counter) + ".json");
		if (ifs.fail()) break;
		Level* level = loader.load("data/levels/rts/" + to_string(level_counter) + ".json", &queue_objects, &text_objects, level_counter);

		for (Moveable* m : level->objects) {
			double* location = m->getLocation();
			if (m->getFlags() & QUAD) {
				vector<map<double, double>> points = m->getPoints();
				Collidable* c = (Collidable*)(m);
				for (int x = 0; x < 4; x++) points[x][0] += level_counter;
				c->setPoints(points[0], points[1], points[2], points[3]);
			}
			m->setLocation(location[0] + level_counter, location[1]);
			registerObject(m);
		}

		for (Text* t : level->text_objects) registerObject(t);
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
		ProvinceMV* province = Loader::getProvinceMap()[id];
		ProvinceMV* other_province = Loader::getProvinceMap()[other_id];
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
		Loader::getProvinceMap()[id]->setSize(w, h);
		Loader::getProvinceMap()[id]->setLocation(x + xoffset, y + yoffset);
	}
	fclose(dimensions_file);

	vector<ProvinceMV*> path;
	// path.push_back(Loader::getProvinceMap()[5]);
	path.push_back(Loader::getProvinceMap()[22]);
	path.push_back(Loader::getProvinceMap()[21]);
	path.push_back(Loader::getProvinceMap()[20]);
	path.push_back(Loader::getProvinceMap()[19]);
	path.push_back(Loader::getProvinceMap()[17]);
	path.push_back(Loader::getProvinceMap()[18]);
	Loader::getUnitMap()[1]->setPath(path);
	Loader::getUnitMap()[1]->initiate();

	for (const auto& p : Loader::getNationMap()) {
		Nation* nation = p.second;
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

	/*
	player.setColour(236, 240, 241, 175);
	player.setLocation(0.075, 0.6);
	player.setSize(player.getSize()[0] * 4, player.getSize()[1] * 4);
	player.addFlag(TEXTURED);
	player.setTexture(Image::getImage("img.png"));
	player.loadScript("data/switcher.txt"); // preload script img/fonts
	*/

	for (Text* t : text_objects) t->addFlag(BUTTON);

	keyboard = new Keyboard(this);
	mouse = new Mouse(this);

	glfwSetKeyCallback(window, keyboard->callback);
	glfwSetMouseButtonCallback(window, mouse->callback);
	glfwSetScrollCallback(window, mouse->scroll_callback);

	info("Took " + to_string(glfwGetTime() - launch_time) + " seconds to load the game.");
}

vector<Unit*> Game::getUnitsOnProvince(ProvinceMV* province) {
	vector<Unit*> stationed_units;
	for (const auto& p : Loader::getUnitMap()) {
		Unit* unit = p.second;
		if (unit->getProvince() == province) {
			stationed_units.push_back(unit);
		}
	}
	return stationed_units;
}

/*
void gameLogic() {
	for (const auto& p : Loader::getProvinceMap()) {
		ProvinceMV* province = p.second;
		vector<Unit*> province_units = getUnitsOnProvince(province);
		if (province_units.size() >= 2) {
			for (Unit* unit : province_units) {
				for (Unit* other_unit : province_units) {
					if (unit->getNation() != other_unit->getNation)
				}
			}
		}
	}
}
*/

Player* Game::getPlayer() { return &player; }

bool Game::isWithin(Moveable* m, double x, double y) {
	double* location = m->getLocation();
	double* size = m->getSize();
	return x > location[0] && x < location[0] + size[0] && y > location[1] && y < location[1] + size[1];
}

bool Game::isWithin(double x1, double y1, double x2, double y2, double x, double y) { return x > x1 && x < x2 && y > y1 && y < y2; }

void Game::checkCollision() {
	double* location = player.getLocation();
	double* size = player.getSize();
	double x, y;

	for (Moveable* m : objects) {
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
		} else if (m->getFlags() & COLLIDABLE) {
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
}

void Game::fireShot() {
	Ice* ice = new Ice();
	ice->setLocation(player.getLocation()[0], player.getLocation()[1]);
	ice->setAcceleration(0.0001, 0);
	ice->objects = &queue_objects;
	registerObject(ice);
}

void Game::debugMode() {
	render.scale = 0.75;
	render.internal_scale_w = 0.75;
	render.internal_scale_h = 0.75;
	debug = true;
	secondary_objects.clear();
	int counter = 0;
	for (Moveable* m : objects) {
		string moveable_name = m->getName();
		for (auto& c : moveable_name) c = toupper(c);
		Text* d = new Text(0.055, 0.025 + 0.0525 * counter, Fonts::getFont(CONSOLAS_BOLD, 12), 189, 195, 199, 150, moveable_name);
		d->addFlag(BUTTON);
		secondary_objects.push_back(d);
		counter++;
	}
}

void Game::checkTimers() {
	double current_time = glfwGetTime();
	for (map<double, int> timed_task : timers_);
}

void Game::updateStatistics(int f, int u) {
	// TODO: Only update mem values if console is visible
	PROCESS_MEMORY_COUNTERS memCounter;
	BOOL result = K32GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));
	stringstream ss;
	string t_time = to_string(render.draw_times[1] * 1000), s_time = to_string(render.draw_times[0] * 1000), u_time = to_string(update_time_ * 1000);
	t_FPSCounter.setContent("FPS: " + to_string(f));
	t_Alt.setContent("T: " + t_time.substr(0, t_time.size() - 2) + "ms");
	t_Alt2.setContent("S: " + s_time.substr(0, s_time.size() - 2) + "ms");
	t_Alt3.setContent("U: " + u_time.substr(0, u_time.size() - 2) + "ms");
	console->update("Objects: " + to_string(objects.size()) + "    Update Rate: " + to_string(update_rate) + "    Memory: " + to_string(memCounter.WorkingSetSize / 1048576) + "MB");
	// t_PlayerAcceleration.setContent("ACL: " + to_string(player.getAcceleration()[0]) + ", " + to_string(player.getAcceleration()[1]));
	// t_PlayerVelocity .setContent("VEL: " + to_string(player.getVelocity()[0]) + ", " + to_string(player.getVelocity()[1]));
	// t_Notification.setContent("");

	string nation_string = "-- [X]";
	if (player_nation) {
		nation_string = player_nation->getName() + " [" + to_string(player_nation->getID()) + "]";
		t_PlayerAcceleration.setColour(player_nation->colour[0], player_nation->colour[1], player_nation->colour[2], 255);
	}

	// font_data font = Fonts::getFont("data/fonts/brockscript.ttf", 25, true);
	// t_PlayerAcceleration.setFont(font);


	t_PlayerLocation.setContent("Day #" + to_string(elapsed_days));
	t_PlayerVelocity.setContent("Money: 0.00");
	t_PlayerAcceleration.setContent("Playing as: " + nation_string);
	info(ss << "FPS: " << f << " \tUpdates: " << u << " \tGame time: " << update_time_ << "ms \t[" << (int)(1 / update_time_) << "]");
}

void Game::updateProperties() {
	double x, y, relx, rely; 	// Can be heavily optimised.
	int dimensions[2] = { 1280, 720 };
	glfwGetWindowSize(window, &dimensions[0], &dimensions[1]);
	glfwGetCursorPos(window, &x, &y);
	relx = x / dimensions[0], rely = y / dimensions[1];

	if (selected_object) {
		if (selected_object->getFlags() & PROVINCE) {
			selected_object->Moveable::setColour(255, 255, 0);
			ProvinceMV* province = (ProvinceMV*)selected_object;
			for (ProvinceMV* neighbour : province->getNeighbours()) {
				neighbour->Moveable::setColour(255, 0, 255);
			}
		}
		else if (selected_object->getFlags() & UNIT) { // t: enable flag in loader

		}
	}

	if (rbDown && selected_object) {
		double size[2] = { abs(game->mouse_position[0] - relx), abs(game->mouse_position[1] - rely) };
		selected_object->setSize(size[0], size[1]);
		t_Notification.setContent("Set size of " + game->selected_object->getName() + " to " + to_string(size[0]) + ", " + to_string(size[1]));
	}
	if (mbDown) render.offsets[0] += (relx - original_position[0]) * 0.01, render.offsets[1] += (rely - original_position[1]) * 0.01;
	/*if (!rbDown && !mbDown) {
		selected_button = nullptr;
		selectButton(text_objects, relx, rely);
		selectButton(secondary_objects, relx, rely);
	}*/ // DISABLED
}

void Game::selectButton(vector<Text*> objects, double x, double y) {
	for (Text* t : objects) // Broken.
		if (t->getFlags() & BUTTON) {
			if (t->getFlags() & SELECTED) t->removeFlag(SELECTED);
			if (t->getCollisionBox() != nullptr)
				if (isWithin(t->getCollisionBox(), x, y)) {
					t->addFlag(SELECTED);
					selected_button = t;
					break;
			}
		}
}

void Game::updateObjects(double modifier) {
	// Sidescroller movement code
	/*
	double x_location = player.getLocation()[0], movement = 0;
	if ((traversed && x_location <= 0.3) || x_location >= 0.65)
		if (x_location >= 0.65) {
			if (!traversed) traversed = true;
			movement = x_location - 0.65;
		} else movement = -(0.3 - x_location);

		for (Moveable* m : objects) {
			if (m->getFlags() & FIXED_POS) continue;
			double* location = m->getLocation();
			m->setLocation(location[0] - movement, location[1]);
			// render.uoffsets[0] = location[0] - movement;

			if (m->getFlags() & QUAD) {
				Collidable* c = (Collidable*)m;
				vector<map<double, double>> points = c->getPoints();
				for (int x = 0; x < 4; x++) points[x][0] -= movement;
				c->setPoints(points);
			}
		}

		for (Text* t : text_objects) {
			if (t->fixed) continue;
			double* location = t->getLocation();
			t->setLocation(location[0] - movement, location[1]);
		}
	*/

	vector<Moveable*> temp;
	for (Moveable* m : objects) !m->isActive ? temp.push_back(m) : m->update(modifier);

	for (Moveable* t : temp) {
		objects.erase(remove(objects.begin(), objects.end(), t), objects.end());
		delete t;
	}

	for (Moveable* q : queue_objects) objects.push_back(q);
	queue_objects.clear();
	// if (player.getLocation()[0] >= 0.99) transitionLevel(); -- removed.
}

void Game::registerObject(Moveable* m) { objects.push_back(m); }

void Game::registerObject(Text* t) { text_objects.push_back(t); }

void Game::transitionLevel() {
	level_index++;
	objects = levels[level_index].objects;
	registerObject(&player);
	player.setLocation(0.05, 0.65);
}

int Game::gameLoop() {
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
			for (Nation* nation : nations) nation->evaluate();
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
	for (Moveable* t : objects) {
		if (t->getFlags() & SQUARE) delete t;
		counter++;
	} objects.clear();

	for (const auto& p : Image::images) {
		Texture* texture = p.second;
		delete[] texture->image;
		delete texture;
	}

	stringstream ss;
	info(ss << "Average FPS after " << frame_count << " seconds: " << average_frames);
	return 0;
}
