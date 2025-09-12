#include "game_sidescroller.h"

#include "io/mouse.h"
#include "io/keyboard.h"
#include "assets/particle_group.h"
#include "assets/player.h"
#include "assets/text.h"
#include "tools/text_renderer.h"
#include "tools/image.h"
#include "tools/console.h"
#include "tools/render.h"

// #define DEBUG_PROFILING

using namespace std;

void GameSidescroller::initialise_extended() {
	player = new Player();
	player->set_colour(Colour(236, 240, 241, 175));
	player->set_location(0.075f, 0.6f);
	player->size = { 80.0f / WINDOW_WIDTH, 80.0f / WINDOW_HEIGHT };
	player->add_flag(TEXTURED);
	player->set_texture(Image::get_image("img.png"));
	player->load_script("data/scripts/switcher.txt");
	mouse->debug_control_scheme = true;

	register_object(player);
	load_levels("data/levels/sidescroller/");

	console->build();

	log_t("Took " CON_RED, glfwGetTime() - launch_time_, " seconds " CON_NORMAL "to load the game.");
}

void GameSidescroller::check_collision() {
	Vector2 location = player->get_location();
	Vector2 size = player->get_size();
	// float x, y;

	for (Moveable* m : objects) {
		Vector2 m_location = m->get_location();
		Vector2 m_size = m->get_size();
		/*if (m->get_flags() & GHOST) continue;
		else if (m->get_flags() & QUAD) { // STAIRS
			map<float, float> first_point = m->get_points()[0];
			map<float, float> second_point = m->get_points()[1];
			if (first_point[0] - size[0] / 2 < location[0] && location[0] < second_point[0] - size[0] / 2) {
				float xdiff = abs(second_point[0] - first_point[0]);
				float ydiff = abs(second_point[1] - first_point[1]);
				float relx = abs((location[0] + size[0] / 2) - first_point[0]) / xdiff;
				float ground = (first_point[1] - (ydiff * relx)) - size[1];
				if (location[1] >= ground) {
					player.set_location(location[0] + m->get_velocity().x, ground + m->get_velocity().y);
					player.set_velocity(player.get_velocity().x, 0);
					return;
				}
			}
		}
		else */if (m->get_flags() & COLLIDABLE) {
			if (m_location.x - size.x / 2 < location.x && location.x < m_location.x + m_size.x - size.x / 2)
				if (location.y + size.y >= m_location.y && location.y + size.y <= m_location.y + 0.005f) {
					player->set_location(location.x + m->get_velocity().x, m_location.y - size.y);
					player->set_velocity(player->get_velocity().x, 0);
					return;
				}
		}
	}
	if (player->get_location().y >= 1.0f && !god_mode) {
		player->set_location(0.05f, 0.3f);
	}
	player->set_velocity(player->get_velocity().x, GRAVITY);
}

void GameSidescroller::update_objects(const float& modifier) {
	double x_location = player->get_location().x, movement = 0;
	if ((traversed && x_location <= 0.3f) || x_location >= 0.65f)
		if (x_location >= 0.65f) {
			if (!traversed) traversed = true;
			movement = x_location - 0.65f;
		} else movement = -(0.3f - x_location);

	for (Moveable* m : objects) {
		if (m->get_flags() & FIXED_POS) continue;
		m->location.x -= movement;
		// render->uoffsets[0] = location[0] - movement;

		/*
		if (m->get_flags() & QUAD) {
			Collidable* c = (Collidable*)m;
			vector<map<double, double>> points = c->get_points();
			for (int x = 0; x < 4; x++) points[x][0] -= movement;
			c->setPoints(points);
		}
		*/
	}

	for (Text* t : text_objects) {
		if (t->has_flag(FIXED_POS)) continue;
		t->location.x -= movement;
	}

	vector<Moveable*> temp;
	for (Moveable* m : objects) !m->is_active ? temp.push_back(m) : m->update(modifier);

	for (Moveable* t : temp) {
		objects.erase(remove(objects.begin(), objects.end(), t), objects.end());
		delete t;
	}

	for (Moveable* q : queue_objects) objects.push_back(q);
	queue_objects.clear();
}

int GameSidescroller::game_loop() {
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
			check_collision();
			update_properties();
			glfwPollEvents();
			updates++;
			delta_time--;
			update_time_ = glfwGetTime() - update_time;
		}

		if (fps_limit == 0 || glfwGetTime() - last_frame_time >= (1.0f / fps_limit)) {
			last_frame_time = glfwGetTime();
			render->render_window();
			frames++;
		}

		if (glfwGetTime() - timer > 1.0f) {
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
