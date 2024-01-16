#pragma once

class Circle;
class Text;

using namespace std;
using namespace glfreetype;

class Render {
	private:
		GLFWwindow* window_ = nullptr;
		vector<Moveable*>* objects_ = nullptr;
		vector<Text*>* text_objects_ = nullptr;
		bool fullscreen_ = false;
		float render_level_ = 0;

	public:
		Vector2 resolution = { 1280, 720 };
		Vector2 offsets;
		float scale = 1.00;
		float draw_times[3]{};

		Render() {}
		Render(GLFWwindow*, vector<Moveable*>*, vector<Text*>*);

		void drawQuad(Vector2 location, Vector2 size, Colour colour, Colour gradient);

		void drawTexture(Vector2 location, Vector2 size, Texture* texture, Colour colour, bool flip = false);

		void drawCurvedQuad(Vector2 location, Vector2 size, Colour colour, Colour gradient, float radius = 0.025);

		void drawCustom(vector<Vector2> points, Colour colour, Colour gradient);

		void drawText(Vector2 location, string message, font_data font, Colour colour);

		void drawCircle(Circle*);
		void drawCircle(Vector2 location, Colour colour, Colour gradient, float radius, float generality);

		void renderWindow();
		void toggleFullscreen();
		void normaliseCoordinates(Vector2* location);

		void setRenderLevel(float level) { render_level_ = level; }
};
