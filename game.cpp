#ifdef _WIN32
#include <windows.h>
#include <Psapi.h>
#endif

#include <filesystem>
#include <vector>
#include <algorithm>
#include <fstream>

#include "game.h"
#include "io/mouse.h"
#include "io/keyboard.h"
#include "io/audio.h"

#include "assets/panel.h"

// #define DEBUG_PROFILING

using namespace std;
namespace fs = std::filesystem;

Game* Game::game = nullptr; // Switch to using smart pointers

Font* debug_font = nullptr;

Game::Game(int argc, char** argv) {
	launch_time_ = glfwGetTime();
	game = this;

	if (!glfwInit()) return;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Generic Game", NULL, NULL))) { glfwTerminate(); return; }

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

	debug_font =			Fonts::getFont("data/fonts/consolab.ttf", 11, true);
	t_FPSCounter =			Text({0.925f, 0.050f}, debug_font, Colour(0, 206, 201, 255), "FPS: --");
	t_Information =			Text({0.030f, 0.050f}, debug_font, Colour(34, 166, 179, 255), "");
	t_Information2 =		Text({0.030f, 0.075f}, debug_font, Colour(34, 166, 179, 255), "");
	t_Information3 =		Text({0.030f, 0.100f}, debug_font, Colour(34, 166, 179, 255), "");
	t_Alt =					Text({0.675f, 0.050f}, debug_font, Colour(223, 249, 251, 255), "ALT: --");
	t_Alt2 =				Text({0.800f, 0.050f}, debug_font, Colour(223, 249, 251, 255), "ALT: --");
	t_Alt3 =				Text({0.550f, 0.050f}, debug_font, Colour(223, 249, 251, 255), "ALT: --");
	t_Notification =		Text({0.175f, 0.050f}, debug_font, Colour(189, 195, 199, 255), "");

	registerObject(&t_FPSCounter);
	registerObject(&t_Alt);
	registerObject(&t_Alt2);
	registerObject(&t_Alt3);
	registerObject(&t_Notification);
	registerObject(&t_Information2);
	registerObject(&t_Information3);
	registerObject(&t_Information);

	glfwSetKeyCallback(window, keyboard->callback);
	glfwSetMouseButtonCallback(window, mouse->callback);
	glfwSetScrollCallback(window, mouse->scroll_callback);
	// glfwSetWindowSizeCallback(window, NULL);

	Audio::init();

	log_t("Took " CON_RED, glfwGetTime() - launch_time_,  " seconds " CON_NORMAL "to load the base game.");
}

void Game::loadLevels(string level_directory) {
	int level_counter = 0;
	vector<fs::directory_entry> entries;

	for (const auto& entry : fs::directory_iterator(level_directory)) {
		if (entry.is_regular_file() && entry.path().extension() == ".json") {
			if (isdigit(entry.path().filename().string()[0])) {
				entries.push_back(entry);
			}
		}
	}

	sort(entries.begin(), entries.end());

	for (const fs::directory_entry& entry : entries) {
		Level* level = loader.load_level(entry.path().string(), &queue_objects, &text_objects, level_counter);

		for (Moveable* m : level->objects) {
			if (level->offset_positions) m->location.x += level_counter;
			registerObject(m);
		}

		for (Text* t : level->text_objects) registerObject(t);
		levels.push_back(*level);
		level_counter++;
		delete level;
	}
}

static int paused = 0;
static ColourShift fadeShift(Colour first_colour, Colour second_colour, bool swap, ColourShift::DIRECTION = ColourShift::DIRECTION::UP) {
	ColourShift colourshift = ColourShift(first_colour, second_colour);
	if (swap) colourshift.reswap();
	colourshift.setCondition(&paused);
	colourshift.loop = false;
	colourshift.speed = 0.03f;
	colourshift.with_gradient = true;
	return colourshift;
}

void Game::debugMode() { UIManager::Toggle("ui_menu_pause"); }

void Game::toggleDebug() {
	mouse->debug_control_scheme ^= true;
	t_Notification.setContent("Updated control scheme to: " + to_string(mouse->debug_control_scheme));
}

void Game::updateStatistics(int f, int u) {
	if (console->visible()) {
#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS memCounter;
		BOOL result = K32GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));
		console->update("OBJ: " + to_string(objects.size()) + " (skip: " + to_string(render.skipped_count) + " cull: " + to_string(render.culled_count) + " text: +" + to_string(text_objects.size())  + ")" + "  UPD: " + to_string(update_rate) + "  MEM: " + to_string(memCounter.WorkingSetSize / 1048576) + "MB");
#else
		console->update("Objects: " + to_string(objects.size()) + " (skip: " + to_string(render.skipped_count) + " cull: " + to_string(render.culled_count) + ")" + "  Updates: " + to_string(update_rate));  // there are linux alternatives
#endif
	}

	string t_time = to_string(render.draw_times[1] * 1000), s_time = to_string(render.draw_times[0] * 1000), u_time = to_string(update_time_ * 1000);
	t_FPSCounter.setContent("FPS: " + to_string(f));
	t_Alt.setContent("T: " + t_time.substr(0, t_time.size() - 2) + "ms");
	t_Alt2.setContent("S: " + s_time.substr(0, s_time.size() - 2) + "ms");
	t_Alt3.setContent("U: " + u_time.substr(0, u_time.size() - 2) + "ms");

	log_t("FPS: " CON_RED, f, CON_NORMAL " \tUpdates: " CON_RED, u, CON_NORMAL " \tGame time: " CON_RED, update_time_,  "s" CON_NORMAL "\t[", CON_RED, (int)(1 / update_time_), CON_NORMAL "]");
}

void Game::updateCursor() {
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	cursor_position.set(x / render.resolution.x, y / render.resolution.y);
}

void Game::updateProperties() {
	updateCursor();

	if (getButton(GLFW_MOUSE_BUTTON_MIDDLE)) {
		render.offsets.x += (cursor_position.x - original_position.x) * 0.01, render.offsets.y += (cursor_position.y - original_position.y) * 0.01;
	}

	if (!selected_object) return;

	if (mouse->debug_control_scheme) {
		if (getButton(GLFW_MOUSE_BUTTON_RIGHT)) {
			Vector2 new_size = Vector2(abs(game->mouse_position.x - cursor_position.x), abs(game->mouse_position.y - cursor_position.y));
			selected_object->setSize(new_size.x, new_size.y);
			t_Notification.setContent("Set size of " + game->selected_object->getName() + " to " + to_string(new_size.x) + ", " + to_string(new_size.y));
		}
	}
}

void Game::updateObjects(float modifier) {
	vector<Moveable*> inactive_objects;
	// #pragma omp parralel for num_threads(12)
	for (Moveable* m : objects) !m->is_active ? inactive_objects.push_back(m) : m->update(modifier);
	for (Moveable* to : text_objects) !to->is_active ? inactive_objects.push_back(to) : to->update(modifier);

	for (Moveable* t : inactive_objects) {
		objects.erase(remove(objects.begin(), objects.end(), t), objects.end());
		delete t;
	}

	// PANEL OBJECTS NOT UPDATED OR ARE BECAUSE THEY ITERATE OVER BUNDLE?

	for (Moveable* q : queue_objects) objects.push_back(q);
	queue_objects.clear();
}

void Game::registerObject(Moveable* m) { 
	objects.push_back(m);
	if (m->getFlags() & PANEL) {
		Panel* panel = reinterpret_cast<Panel*>(m);
		for (Moveable* moveable : *panel->get()) {
			moveable->addFlag(UI);
			if (moveable->getFlags() & TEXT)
				continue;
				//registerObject(reinterpret_cast<Text*>(moveable));
			else
				continue;  // WE WERE ADDING OBJECTS TWICE HERE FIX THIS!!!
				//registerObject(moveable);
		}
	}
}

void Game::registerObject(Text* t) { 
	text_objects.push_back(t); 
	if (t->getFont() == debug_font) t->addFlag(DEBUG); // temp
}

static bool within(const Vector2& location, const Vector2& size, const Vector2& point) {
	return point.x > location.x && point.x < location.x + size.x && point.y > location.y && point.y < location.y + size.y;
}

Moveable* Game::getObjectUnderMouse() {
	Moveable* object = nullptr;
	float min_distance = 100;
	for (Moveable* moveable : game->objects) {
		if (!(moveable->getFlags() & (UNEDITABLE | FIXED_POS | DISABLED))) {
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

	hovered_object = object;
	return object;
}

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
			updateObjects(60.0 / update_rate);
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
