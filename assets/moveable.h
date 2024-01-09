#ifndef MOVEABLE_H
#define MOVEABLE_H
#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include "../tools/common.h"
#include "../tools/image.h"

#include "../defs/colour.h"



#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

using namespace std;
class Text;
class Game;


class Moveable {
	public:
		string name = "Generic Moveable";
		vector<string> script;
		Texture* texture;
		glm::mat4 matrix2;
		Text* text;
		static Game* game_instance;

		int type = 0;
		int script_line = 0;
		int flags = ENABLED;

		double timer = 0;

		double acceleration[2] = { 0.0, 0.0 };
		double velocity[2] = { 0.0, 0.0 };
		double location[2] = { 0.0, 0.0 };
		double bounds[2] = { 0.0, 0.0 };
		double size[2] = { 20.0 / 1280, 20.0 / 720 };
		double gl_colour[4] = { 1.0, 1.0, 1.0, 1.0 };
		double gl_gradient[4] = { -1.0, -1.0, -1.0, -1.0 };

		double centre_[2] = {0.0, 0.0};

		short colour[4] = { 255, 255, 255, 255 };
		short default_colour[4] = { 255, 255, 255, 255 };
		short alternate_colour[4] = { 255, 255, 255, 255 };
		short gradient_colour[4] = { -1, -1, -1, -1 };

		short alpha_limit = 255;

		bool shifting_colour = false, shiftingAlpha = false, alphaDownwards = true, oneWay = false, isActive = true;

		Text* getText();
		Texture* getTexture();
		string getName();
		virtual vector<map<double, double>> getPoints();
		virtual double* getAcceleration();
		double* getVelocity();
		double* getLocation();
		double* getSize();
		double* getCentre();
		double* getBounds();
		int getFlags();

		// TODO: Change to Colour2 class
		double* getGLGradient();
		double* getGLColour();
		short* getColour();

		void setText(Text*);
		void setBounds(double x, double y);
		void setTexture(Texture*);
		void setName(string);
		void setAcceleration(double, double);
		void setVelocity(double, double);
		virtual void setLocation(double, double);
		virtual void setSize(double, double);

		void setColour(short*, short = 255);
		void setColour(short, short, short, short = 255);
		void setDefaultColour(short, short, short, short = 255);
		void setAlternateColour(short, short, short, short = 255);
		void setGradientColour(short*, short = 255);
		void setGradientColour(short, short, short, short = 255);
		void updateColour(short*, int, int = 1);
		void resetColour();

		void updateMatrix2();
		glm::mat4 getMatrix2();

		void setColourShifting(bool);
		void setAlphaShifting(bool, bool = false);
		void shiftColour(int = 1);
		void shiftAlpha(double = 0.01);

		void addFlag(int);
		void removeFlag(int);

		void tickTimer(double);
		void loadScript(string);

		void common(double);
		virtual void update(double = 1.0);

		static void setGameInstance(Game* game) {
			game_instance = game;
		}

};

#endif
