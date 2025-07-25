#pragma once

using namespace std;

struct QuadData {
	Vector2 location;
	Vector2 size;
	Colour* colour;
	Colour* gradient;
	float priority;
	float radius;
	bool fixed_position;

	QuadData() : colour(nullptr), gradient(nullptr) {}

	QuadData(Vector2 location, Vector2 size, Colour* colour, Colour* gradient, float priority, float radius = 0.0f, bool fixed_position = false)
		: location(location), size(size), colour(colour), gradient(gradient), priority(priority), radius(radius), fixed_position(fixed_position) {}
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

		float time = glfwGetTime();

		bool fullscreen_ = false;
		float render_level_ = 0;

	public:
		Vector2 resolution = { WINDOW_WIDTH, WINDOW_HEIGHT };
		Vector2 offsets;
		float scale = 1.00;
		float draw_times[3]{};
		int skipped_count = 0;
		int culled_count = 0;

		Render() {}
		Render(GLFWwindow*, vector<Moveable*>*, vector<Text*>*);

		void drawQuadBatch();
		void drawQuadB(Vector2 location, Vector2 size, Colour* colour, Colour* gradient, float priority, float radius = 0.0f, bool fixed_position = false) {
			batched_quads_[quad_count_++] = QuadData(location, size, colour, gradient, priority, radius, fixed_position);
		}

		void drawTextureBatch();
		void drawTextureB(Vector2 location, Vector2 size, Texture* texture, Colour* colour, Blend* blend, const bool& fixed_position = false, Texture* secondary_texture = nullptr, Colour* secondary_colour = nullptr) {
			batched_textures_[texture_count_++] = TextureData(location, size, texture, colour, blend, fixed_position, secondary_texture, secondary_colour);
		}

		void drawCustom(vector<Vector2> points, Colour colour, Colour gradient);

		void drawText(Vector2 location, string& message, Font* font, Colour& colour, float scale = 1.0f, float priority = 0.0f);

		void renderMoveable(Moveable* moveable);
		void renderWindow();
		void toggleFullscreen();
		void normaliseCoordinates(Vector2* location);
		void alignCoordinates(Vector2* location, Vector2* size);
		void setRenderLevel(float level) { render_level_ = level; }
};
