#pragma once

#include "../assets/colour.h"

#include <vector>
#include <string>

using namespace std;

class Moveable;
class Text;
struct GLFWwindow;
struct Texture;
struct Blend;
struct Font;

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

struct QuadData {
	Vector2 location;
	Vector2 size;
	Colour* colour = nullptr;
	Colour* gradient = nullptr;
	float radius;
	bool fixed_position;

	QuadData() : colour(nullptr), gradient(nullptr) {}

	QuadData(Vector2& location, Vector2& size, Colour* colour, Colour* gradient, float radius = 0.0f, bool fixed_position = false)
		: location(location), size(size), colour(colour), gradient(gradient), radius(radius), fixed_position(fixed_position) {}
};

struct TextureData {
	Vector2 location;
	Vector2 size;
	Texture* texture;
	Colour* colour;
	bool fixed_position;
	Texture* secondary_texture;
	const Colour* secondary_colour;
	Blend* blend;

	TextureData() : texture(nullptr), colour(nullptr), fixed_position(false), secondary_texture(nullptr), secondary_colour(nullptr), blend(nullptr) {}

	TextureData(Vector2 location, Vector2 size, Texture* texture, Colour* colour, Blend* blend, bool fixed_position = false, Texture* secondary_texture = nullptr, Colour* secondary_colour = nullptr)
		: location(location), size(size), texture(texture), colour(colour), fixed_position(fixed_position), secondary_texture(secondary_texture), secondary_colour(secondary_colour), blend(blend) {}
};

class Render { // TODO: Switch from immediate mode to direct mode rendering -- update: mostly done
	private:
		GLFWwindow* window_ = nullptr;
		vector<Moveable*>* objects_ = nullptr;
		vector<Text*>* text_objects_ = nullptr;

		QuadData batched_quads_[512];
		TextureData batched_textures_[512];
		
		int quad_count_ = 0;
		int texture_count_ = 0;
		bool fullscreen_ = false;

	public:
		Vector2 resolution = { WINDOW_WIDTH, WINDOW_HEIGHT };
		Vector2 offsets;
		float scale = 1.00;
		float draw_times[3]{};
		int skipped_count = 0;
		int culled_count = 0;
		int render_level = 0;

		Render() {}
		Render(GLFWwindow*, vector<Moveable*>*, vector<Text*>*);

		void draw_batched_quads();
		void draw_quad(Vector2 location, Vector2 size, Colour* colour, Colour* gradient, float radius = 0.0f, bool fixed_position = false) {
			QuadData& q = batched_quads_[quad_count_++];
			q.location = location;
			q.size = size;
			q.colour = colour;
			q.gradient = gradient;
			q.radius = radius;
			q.fixed_position = fixed_position;
		}

		void draw_batched_textures();
		void draw_texture(Vector2 location, Vector2 size, Texture* texture, Colour* colour, Blend* blend, const bool& fixed_position = false, Texture* secondary_texture = nullptr, Colour* secondary_colour = nullptr) {
			TextureData& texture_data = batched_textures_[texture_count_++];
			texture_data.location = location;
			texture_data.size = size;
			texture_data.texture = texture;
			texture_data.colour = colour;
			texture_data.fixed_position = fixed_position;
			texture_data.secondary_texture = secondary_texture;
			texture_data.secondary_colour = secondary_colour;
			texture_data.blend = blend;
		}

		void draw_custom(vector<Vector2> points, Colour colour, Colour gradient);

		void draw_text(Vector2 location, const string& message, Font* font, const Colour& colour, const float& scale = 1.0f);

		void render_moveable(Moveable* moveable);
		void render_window();
		void toggle_fullscreen();
};
