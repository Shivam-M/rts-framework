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

struct CommonUniforms { int colour, colour_secondary, projection; };
struct QuadUniforms : CommonUniforms { int radius; };
struct TextureUniforms : CommonUniforms { int texture, time, type, speed, size, direction; };

struct QuadData {
	Vector2 location{};
	Vector2 size{};
	Colour* colour = nullptr;
	Colour* gradient = nullptr;
	float radius = 0.0f;
	bool fixed_position = false;
	QuadData() = default;
};

struct TextureData {
	Vector2 location{};
	Vector2 size{};
	Colour* colour = nullptr;
	Colour* secondary_colour = nullptr;
	Texture* texture = nullptr;
	Blend* blend = nullptr;
	bool fixed_position = false;
	TextureData() = default;
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

		Render() = default;
		Render(GLFWwindow* window, vector<Moveable*>* objects, vector<Text*>* text_objects);

		void draw_batched_quads();
		void draw_quad(const Vector2& location, const Vector2& size, Colour* colour, Colour* gradient, float radius = 0.0f, bool fixed_position = false) {
			QuadData& quad_data = batched_quads_[quad_count_++];
			quad_data.location = location;
			quad_data.size = size;
			quad_data.colour = colour;
			quad_data.gradient = gradient;
			quad_data.radius = radius;
			quad_data.fixed_position = fixed_position;
		}

		void draw_batched_textures();
		void draw_texture(const Vector2& location, const Vector2& size, Texture* texture, Colour* colour, Blend* blend, bool fixed_position = false, Colour* secondary_colour = nullptr) {
			TextureData& texture_data = batched_textures_[texture_count_++];
			texture_data.location = location;
			texture_data.size = size;
			texture_data.texture = texture;
			texture_data.colour = colour;
			texture_data.fixed_position = fixed_position;
			texture_data.secondary_colour = secondary_colour;
			texture_data.blend = blend;
		}

		void draw_custom(vector<Vector2>* points, Colour* colour, Colour* gradient);

		void draw_text(const Vector2& location, const string& message, Font* font, const Colour& colour, float scale = 1.0f) const;

		void fill_vertex_buffer(int& count, const Vector2& location, const Vector2& size, bool fixed_position) const;

		void render_moveable(Moveable* moveable);
		void render_window();
		void toggle_fullscreen();
};
