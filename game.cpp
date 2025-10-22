#ifdef _WIN32
#include <windows.h>
#include <Psapi.h>
#endif

#include "game.h"

#include "assets/text_entry.h"
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
#ifdef __aarch64__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Generic Game", NULL, NULL))) { glfwTerminate(); return; }

	glfwSetWindowPos(window, 100, 100);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glClearColor(0.155f, 0.155f, 0.155f, 0.f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	srand(time(nullptr));

	log_t("\033[1;31mOpenGL Version: ", glGetString(GL_VERSION));
	log_t("\033[1;31mGLFW Version: ", glfwGetVersionString());

	render = new Render(window, &objects, &text_objects);
	keyboard = new Keyboard(this);
	mouse = new Mouse(this);
	console = new Console(this);

	global_filter = new ColourFilter(Colour(1, 1, 1, 0.9), ColourFilter::Mode::Multiplication);

	Font* debug_font =		Fonts::get_font("data/fonts/consolab.ttf", 11, true);
	t_FPSCounter =			new Text({0.925f, 0.050f}, debug_font, Colour(0, 206, 201, 255), "FPS: --");
	t_Information =			new Text({0.030f, 0.050f}, debug_font, Colour(34, 166, 179, 255), "");
	t_Information2 =		new Text({0.030f, 0.075f}, debug_font, Colour(34, 166, 179, 255), "");
	t_Information3 =		new Text({0.030f, 0.100f}, debug_font, Colour(34, 166, 179, 255), "");
	t_Alt =					new Text({0.675f, 0.050f}, debug_font, Colour(223, 249, 251, 255), "ALT: --");
	t_Alt2 =				new Text({0.800f, 0.050f}, debug_font, Colour(223, 249, 251, 255), "ALT: --");
	t_Alt3 =				new Text({0.550f, 0.050f}, debug_font, Colour(223, 249, 251, 255), "ALT: --");
	t_Notification =		new Text({0.175f, 0.050f}, debug_font, Colour(189, 195, 199, 255), "");

	register_object(t_FPSCounter);
	register_object(t_Alt);
	register_object(t_Alt2);
	register_object(t_Alt3);
	register_object(t_Notification);
	register_object(t_Information2);
	register_object(t_Information3);
	register_object(t_Information);

	glfwSetKeyCallback(window, keyboard->callback);
	glfwSetMouseButtonCallback(window, mouse->callback);
	glfwSetScrollCallback(window, mouse->scroll_callback);
	glfwSetFramebufferSizeCallback(window, this->framebuffer_size_callback);

	Audio::init();

	log_t("Took " CON_RED, glfwGetTime() - launch_time_,  " seconds " CON_NORMAL "to load the base game.");
}

Game::~Game() {
	for (Moveable* moveable : objects) delete moveable;
	for (Text* text : text_objects) delete text;

	delete mouse;
	delete keyboard;
	delete console;
	delete render;
	delete loader;
	delete global_filter;

	Audio::cleanup();
	Fonts::cleanup();
	Image::cleanup();
}

void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);

	game->render->resolution.x = static_cast<float>(width);
	game->render->resolution.y = static_cast<float>(height);
}

void Game::load_levels(string level_directory) {
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
			register_object(m);
		}

		for (Text* t : level->text_objects) register_object(t);
		levels.push_back(level);
		level_counter++;
		delete level;
	}
}

void Game::load_level_dynamically(const string& level_path, const string& instance_name) {
	Level* level = loader->load_level(level_path, &queue_objects, &text_objects, 0, instance_name);
	if (!level) return;
	for (Moveable* m : level->objects) {
		if (level->offset_positions) m->location.x += levels.size();
		register_object(m);
	}
	for (Text* t : level->text_objects) register_object(t);
	levels.push_back(level);
	delete level;
}

void Game::initialise_extended() {
	console->build();
}

void Game::debug_mode() {
	global_filter->colour.a *= 0.1f;
	// UIManager::toggle("ui_menu_pause");

	if (selected_object) {
		log_t("debug");
	}
}

void Game::toggle_debug() {
	mouse->debug_control_scheme ^= true;
	t_Notification->set_content("Updated control scheme to: " + to_string(mouse->debug_control_scheme));
}

void Game::update_statistics(int f, int u) {
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
	t_FPSCounter->set_content("FPS: " + to_string(f));
	t_Alt->set_content("T: " + t_time.substr(0, t_time.size() - 2) + "ms");
	t_Alt2->set_content("S: " + s_time.substr(0, s_time.size() - 2) + "ms");
	t_Alt3->set_content("U: " + u_time.substr(0, u_time.size() - 2) + "ms");

	log_t("FPS: " CON_RED, f, CON_NORMAL " \tUpdates: " CON_RED, u, CON_NORMAL " \tGame time: " CON_RED, update_time_,  "s" CON_NORMAL "\t[", CON_RED, (int)(1 / update_time_), CON_NORMAL "]");
}

void Game::update_cursor() {
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	cursor_position = { (float)x / render->resolution.x, (float)y / render->resolution.y };
}

void Game::update_properties() {
	update_cursor();

	if (get_button(GLFW_MOUSE_BUTTON_MIDDLE)) {
		render->offsets.x += (cursor_position.x - original_position.x) * 0.01;
		render->offsets.y += (cursor_position.y - original_position.y) * 0.01;
	}

	if (!selected_object) return;

	if (mouse->debug_control_scheme) {
		if (get_button(GLFW_MOUSE_BUTTON_RIGHT)) {
			Vector2 new_size = Vector2(abs(game->mouse_position.x - cursor_position.x), abs(game->mouse_position.y - cursor_position.y));
			selected_object->set_size(new_size.x, new_size.y);
			t_Notification->set_content("Set size of " + game->selected_object->name + " to " + to_string(new_size.x) + ", " + to_string(new_size.y));
		}
	}
}

void Game::update_objects(float modifier) {
	erase_if(objects, [modifier](Moveable* moveable) {
		if (!moveable || !moveable->is_active) {
			delete moveable;
			return true;
		}
		moveable->update(modifier);
		return false;
	});

	erase_if(text_objects, [modifier](Text* text) {
		if (!text || !text->is_active) {
			delete text;
			return true;
		}
		text->update(modifier);
		return false;
	});

	objects.insert(objects.end(), queue_objects.begin(), queue_objects.end());
	queue_objects.clear();
}

void Game::register_object(Moveable* object) { 
	objects.push_back(object);
	object->filters.push_back(global_filter);
}

void Game::register_object(Text* text_object) { 
	text_objects.push_back(text_object);
	text_object->filters.push_back(global_filter);
}

static bool within(const Vector2& location, const Vector2& size, const Vector2& point) {
	return point.x > location.x && point.x < location.x + size.x && point.y > location.y && point.y < location.y + size.y;
}

bool Game::is_cursor_on_texture(Moveable* moveable, const Vector2& cursor_position) {
	Texture* texture = moveable->get_texture();
	if (!texture || !texture->image) return false;

	Vector2 location, size;
	if (moveable->has_flag(FIXED_POS)) {
		location = moveable->get_location();
		size = moveable->get_size();
	}
	else {
		location = moveable->get_location() * render->scale + render->offsets;
		size = moveable->get_size() * render->scale;
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

Moveable* Game::get_object_under_mouse() {
	Moveable* object = nullptr;
	float min_distance = 100.0f;

	for (Moveable* moveable : game->objects) {
		if (moveable->get_flags() & (UNEDITABLE | DISABLED)) continue;

		Vector2 location, size;
		if (moveable->has_flag(FIXED_POS)) {
			location = moveable->get_location();
			size = moveable->get_size();
		} else {
			location = moveable->get_location() * render->scale + render->offsets;
			size = moveable->get_size() * render->scale;
		}

		if (within(location, size, cursor_position)) {
			Vector2 centre = moveable->get_centre() * render->scale + render->offsets;
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
	return hovered_object;
}

#include "tools/text_renderer.h"

int Game::game_loop() {
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
			update_objects(60.0 / update_rate);
			update_properties();
			glfwPollEvents();
			updates++;
			delta_time--;
			update_time_ = glfwGetTime() - update_time;
		}

		if (fps_limit == 0 || glfwGetTime() - last_frame_time >= (1.0 / fps_limit)) {
			last_frame_time = glfwGetTime();
			render->render_window();
			frames++;
		}

		if (glfwGetTime() - timer > 1.0) {
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
