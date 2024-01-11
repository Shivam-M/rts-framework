#pragma once

#include <vector>

#include "common.h"
#include "image.h"

class CircleNew;
class TextNew;

using namespace std;
using namespace glfreetype;

class RenderNew {
	private:
		GLFWwindow* window_;
		vector<MoveableNew*>* objects_;
		vector<TextNew*>* text_objects_;
		Vector2 resolution = { 1280, 720 };
		bool fullscreen_ = false;
		bool debug_ = false;
		int render_level_ = 0;

	public:
		RenderNew() {}
		RenderNew(GLFWwindow*, vector<MoveableNew*>*, vector<TextNew*>*);

		void drawQuad(Vector2 location, Vector2 size, Colour2 colour, Colour2 gradient);

		void drawTexture(Vector2 location, Vector2 size, Texture* texture, Colour2 colour, bool flip = false);

		void drawCurvedQuad(Vector2 location, Vector2 size, Colour2 colour, Colour2 gradient, double radius = 0.025);

		void drawCustom(vector<Vector2> points, Colour2 colour, Colour2 gradient);

		void drawText(Vector2 location, string message, font_data font, Colour2 colour);

		void drawCircle(CircleNew*);
		void drawCircle(Vector2 location, Colour2 colour, Colour2 gradient, double radius, double generality);

		void renderWindow();
		void toggleFullscreen();

		double ang = 0;
		double scale = 1.00;
		double draw_times[3];
		Vector2 offsets;
};
