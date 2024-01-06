#ifndef RENDER_H
#define RENDER_H
#pragma once

#define GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "image.h"
#include "../shapes/circle.h"
#include "../assets/text.h"
#include "../assets/collidable.h"
#include "TextRenderer.hpp"

#include <vector>
#include <map>

using namespace std;
using namespace glfreetype;

class Render {
	private:
		GLuint program, program_textured, program_instanced, program_circled;
		GLFWwindow* window_;
		vector<Moveable*>* objects_;
		vector<Text*>* text_objects_;
		vector<Text*>* secondary_objects_;
		int viewport[4] = {0, 0, 1280, 720};
		bool fullscreen_ = false;
		bool debug_ = false;
		
	public:
		Render();
		Render(GLFWwindow*, vector<Moveable*>*, vector<Text*>*, vector<Text*>*);

		void drawTexture(Moveable*);
		void drawTexture(double*, double*, Texture*, double* = nullptr, bool = false);
		void drawTexture(double, double, double, double, Texture*, double* = nullptr, bool = false);

		void drawQuad(Moveable*);
		void drawQuad(double*, double*, double* = nullptr, double* = nullptr);
		void drawQuad(double, double, double, double, double* = nullptr, double* = nullptr);
		void drawCurvedQuad(double, double, float, float, float = 0.1, double* = nullptr, double* = nullptr);

		void drawCustom(vector<map<double, double>>, double* = nullptr, double* = nullptr);

		void drawCircle(Circle*);
		void drawCircle(double* location, double radius, double* colour, double* gradient_colour, double generality);
		// void drawCircle(double* location, double radius, double* colour, double* gradient_colour, double generality, glm::mat4 matrix);
		// void drawCircle(double*, double, double* = nullptr, double* = nullptr, double = 1.0);
		// void drawCircle(double, double, double, double* = nullptr, double* = nullptr, double = 1.0);
		// void drawCircle(double x, double y, double radius, double* colour, double* gradient_colour, double generality, glm::mat4 matrix);


		// void drawCircle(double* location, double radius, double* colour, double* gradient_colour, double generality, glm::mat4 matrix);
		// void drawCircle(double x, double y, double radius, double* colour, double* gradient_colour, double generality, glm::mat4 matrix);

		void drawCircle(double, double, double, double* = nullptr, double* = nullptr, double = 1.0);
		void drawRing(double, double, double, double* = nullptr, double = 1.00);
		
		void drawText(Text*);
		void drawText(string, double*, font_data, double* = nullptr);
		void drawText(string, double, double, font_data, double* = nullptr);

		void normaliseCoordinates(double*, double*);
		void renderWindow();
		void toggleFullscreen();
		int* retrieveDimensions();
		// void retrieveDimensions(int& width, int &height);

		double draw_times[3];
		double offsets[2] = { 0, 0 };
		double uoffsets[2] = { 0, 0 };
		double scale = 1;
		double internal_scale_w = 1.0;
		double internal_scale_h = 1.0;
		double render_method = 0;

		double ang = 0;
};

#endif
