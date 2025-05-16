#pragma once

#include <deque>

class Circle;

using namespace std;

struct QuadData {
	Vector2 location; Vector2 size; Colour& colour; Colour& gradient; float priority; float radius; bool fixed_position;
};

struct TextureData {
	Vector2 location; Vector2 size; Texture* texture; Colour& colour; bool fixed_position; Texture* secondary_texture; const Colour& secondary_colour; Blend& blend;
};

class Render { // TODO: Switch from immediate mode to direct mode rendering -- update: mostly done
	private:
		GLFWwindow* window_ = nullptr;
		vector<Moveable*>* objects_ = nullptr;
		vector<Text*>* text_objects_ = nullptr;
		deque<QuadData> batched_quads_;
		deque<TextureData> batched_textures_;

		// TextureData all_textures[512];
		// int texture_count = 0;

		float time = glfwGetTime();

		bool fullscreen_ = false;
		float render_level_ = 0;

	public:
		Vector2 resolution = { WINDOW_WIDTH, WINDOW_HEIGHT };
		Vector2 offsets;
		float scale = 1.00;
		float draw_times[3]{};
		int culled_count = 0;

		Render() {}
		Render(GLFWwindow*, vector<Moveable*>*, vector<Text*>*);

		void drawQuadBatch();
		void drawQuadB(Vector2 location, Vector2 size, Colour& colour, Colour& gradient, float priority, float radius = 0.0f, bool fixed_position = false) {
			batched_quads_.push_back({ location, size, colour, gradient, priority, radius, fixed_position });
		}

		void drawTextureBatch();
		void drawTextureB(Vector2 location, Vector2 size, Texture* texture, Colour& colour, Blend& blend, const bool& fixed_position = false, Texture* secondary_texture = nullptr, const Colour& secondary_colour = Colour(-1, -1, -1, -1)) {
			batched_textures_.push_back({ location, size, texture, colour, fixed_position, secondary_texture, secondary_colour, blend });
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
