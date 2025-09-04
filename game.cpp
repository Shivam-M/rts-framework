#ifdef _WIN32
#include <windows.h>
#include <Psapi.h>
#endif

#include "game.h"

#include "assets/text.h"
#include "assets/panel.h"
#include "tools/console.h"
#include "tools/common.h"
#include "tools/image.h"
#include "tools/render.h"
#include "tools/fonts.h"
#include "tools/ui_manager.h"
#include "tools/loader.h"
#include "io/mouse.h"
#include "io/keyboard.h"
#include "io/audio.h"

#include <filesystem>
#include <fstream>

// #define DEBUG_PROFILING

using namespace std;
namespace fs = std::filesystem;

Game* Game::game = nullptr; // Switch to using smart pointers;

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

	render = new Render(window, &objects, &text_objects);
	keyboard = new Keyboard(this);
	mouse = new Mouse(this);
	console = new Console(this);

	global_filter = new ColourFilter(Colour(1, 1, 1, 0.9), ColourFilter::Mode::Multiplication);

	Font* debug_font =		Fonts::getFont("data/fonts/consolab.ttf", 11, true);
	t_FPSCounter =			new Text({0.925f, 0.050f}, debug_font, Colour(0, 206, 201, 255), "FPS: --");
	t_Information =			new Text({0.030f, 0.050f}, debug_font, Colour(34, 166, 179, 255), "");
	t_Information2 =		new Text({0.030f, 0.075f}, debug_font, Colour(34, 166, 179, 255), "");
	t_Information3 =		new Text({0.030f, 0.100f}, debug_font, Colour(34, 166, 179, 255), "");
	t_Alt =					new Text({0.675f, 0.050f}, debug_font, Colour(223, 249, 251, 255), "ALT: --");
	t_Alt2 =				new Text({0.800f, 0.050f}, debug_font, Colour(223, 249, 251, 255), "ALT: --");
	t_Alt3 =				new Text({0.550f, 0.050f}, debug_font, Colour(223, 249, 251, 255), "ALT: --");
	t_Notification =		new Text({0.175f, 0.050f}, debug_font, Colour(189, 195, 199, 255), "");

	registerObject(t_FPSCounter);
	registerObject(t_Alt);
	registerObject(t_Alt2);
	registerObject(t_Alt3);
	registerObject(t_Notification);
	registerObject(t_Information2);
	registerObject(t_Information3);
	registerObject(t_Information);

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
		Level* level = loader->load_level(entry.path().string(), &queue_objects, &text_objects, level_counter);

		for (Moveable* m : level->objects) {
			if (level->offset_positions) m->location.x += level_counter;
			registerObject(m);
		}

		for (Text* t : level->text_objects) registerObject(t);
		levels.push_back(level);
		level_counter++;
		delete level;
	}
}

void Game::dynamicLoadLevel(const string& level_path, const string& instance_name) {
	Level* level = loader->load_level(level_path, &queue_objects, &text_objects, 0, instance_name);
	if (!level) return;
	for (Moveable* m : level->objects) {
		if (level->offset_positions) m->location.x += levels.size();
		registerObject(m);
	}
	for (Text* t : level->text_objects) registerObject(t);
	levels.push_back(level);
	delete level;
}

void Game::extendedInitialisation() {
	console->build();
}

void Game::debugMode() {
	global_filter->colour.w_ *= 0.1f;
	// UIManager::Toggle("ui_menu_pause");
}

void Game::toggleDebug() {
	mouse->debug_control_scheme ^= true;
	t_Notification->setContent("Updated control scheme to: " + to_string(mouse->debug_control_scheme));
}

void Game::updateStatistics(const int& f, const int& u) {
	if (console->visible()) {
#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS memCounter;
		BOOL result = K32GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));
		console->update("OBJ: " + to_string(objects.size()) + " (skip: " + to_string(render->skipped_count) + " cull: " + to_string(render->culled_count) + " text: +" + to_string(text_objects.size())  + ")" + "  UPD: " + to_string(update_rate) + "  MEM: " + to_string(memCounter.WorkingSetSize / 1048576) + "MB");
#else
		console->update("Objects: " + to_string(objects.size()) + " (skip: " + to_string(render->skipped_count) + " cull: " + to_string(render->culled_count) + ")" + "  Updates: " + to_string(update_rate));  // there are linux alternatives
#endif
	}

	string t_time = to_string(render->draw_times[1] * 1000), s_time = to_string(render->draw_times[0] * 1000), u_time = to_string(update_time_ * 1000);
	t_FPSCounter->setContent("FPS: " + to_string(f));
	t_Alt->setContent("T: " + t_time.substr(0, t_time.size() - 2) + "ms");
	t_Alt2->setContent("S: " + s_time.substr(0, s_time.size() - 2) + "ms");
	t_Alt3->setContent("U: " + u_time.substr(0, u_time.size() - 2) + "ms");

	log_t("FPS: " CON_RED, f, CON_NORMAL " \tUpdates: " CON_RED, u, CON_NORMAL " \tGame time: " CON_RED, update_time_,  "s" CON_NORMAL "\t[", CON_RED, (int)(1 / update_time_), CON_NORMAL "]");
}

void Game::updateCursor() {
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	cursor_position.set(x / render->resolution.x, y / render->resolution.y);
}

void Game::updateProperties() {
	updateCursor();

	if (getButton(GLFW_MOUSE_BUTTON_MIDDLE)) {
		render->offsets.x += (cursor_position.x - original_position.x) * 0.01;
		render->offsets.y += (cursor_position.y - original_position.y) * 0.01;
	}

	if (!selected_object) return;

	if (mouse->debug_control_scheme) {
		if (getButton(GLFW_MOUSE_BUTTON_RIGHT)) {
			Vector2 new_size = Vector2(abs(game->mouse_position.x - cursor_position.x), abs(game->mouse_position.y - cursor_position.y));
			selected_object->setSize(new_size.x, new_size.y);
			t_Notification->setContent("Set size of " + game->selected_object->getName() + " to " + to_string(new_size.x) + ", " + to_string(new_size.y));
		}
	}
}

void Game::updateObjects(const float& modifier) {
	erase_if(objects, [modifier](Moveable* moveable) {
		if (!moveable->is_active) {
			delete moveable;
			return true;
		}
		moveable->update(modifier);
		return false;
	});

	erase_if(text_objects, [modifier](Text* text) {
		if (!text->is_active) {
			delete text;
			return true;
		}
		text->update(modifier);
		return false;
	});

	objects.insert(objects.end(), queue_objects.begin(), queue_objects.end());
	queue_objects.clear();
}

void Game::registerObject(Moveable* object) { 
	objects.push_back(object);
	object->filters.push_back(global_filter);
}

void Game::registerObject(Text* text_object) { 
	text_objects.push_back(text_object);
	text_object->filters.push_back(global_filter);
}

static bool within(const Vector2& location, const Vector2& size, const Vector2& point) {
	return point.x > location.x && point.x < location.x + size.x && point.y > location.y && point.y < location.y + size.y;
}

bool Game::cursorPositionOnTexture(Moveable* moveable, const Vector2& cursor_position) {
	Texture* texture = moveable->getTexture();
	if (!texture || !texture->image) return false;

	Vector2 location, size;
	if (moveable->hasFlag(FIXED_POS)) {
		location = moveable->getLocation();
		size = moveable->getSize();
	}
	else {
		location = moveable->getLocation() * render->scale + render->offsets;
		size = moveable->getSize() * render->scale;
	}

	float u = (cursor_position.x - location.x) / size.x;
	float v = (cursor_position.y - location.y) / size.y;

	if (u < 0 || u > 1 || v < 0 || v > 1) return false;

	int x = static_cast<int>(u * texture->width);
	int y = static_cast<int>(v * texture->height);

	x = clamp(x, 0, texture->width - 1);
	y = clamp(y, 0, texture->height - 1);

	int channels = texture->composition;
	int index = (y * texture->width + x) * channels;
	unsigned char alpha = texture->image[index + 3];

	return alpha > 0;
}

Moveable* Game::getObjectUnderMouse() {
	Moveable* object = nullptr;
	float min_distance = 100.0f;

	for (Moveable* moveable : game->objects) {
		if (moveable->getFlags() & (UNEDITABLE | DISABLED)) continue;

		Vector2 location, size;
		if (moveable->hasFlag(FIXED_POS)) {
			location = moveable->getLocation();
			size = moveable->getSize();
		} else {
			location = moveable->getLocation() * render->scale + render->offsets;
			size = moveable->getSize() * render->scale;
		}

		if (within(location, size, cursor_position)) {
			Vector2 centre = moveable->getCentre() * render->scale + render->offsets;
			float dx = centre.x - cursor_position.x;
			float dy = centre.y - cursor_position.y;
			float distance = dx * dx + dy * dy;

			if (distance < min_distance) {
				min_distance = distance;
				object = moveable;
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
			render->renderWindow();
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
			temp_profiling_s.push_back(render->draw_times[0]);
			temp_profiling_t.push_back(render->draw_times[1]);
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
