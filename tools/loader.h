#pragma once

#include <json.hpp>

using namespace nlohmann;
using namespace std;

#include "../defs/colour.h"
#include "../tools/fonts.h"

class Province;
class Nation;
class Unit;
class Moveable;
class Circle;
class Text;
class Collidable;

struct Level {
	Vector2 player_position;
	vector<Moveable*> objects;
	vector<Text*> text_objects;
};

class Loader {
	private:
		// static Fire* parseFire();
		// static Stars* parseStars();
		static Collidable* parseCollidable();
		static Collidable* parseCustom();
		static Moveable* parseMoveable();
		// static Square* parseSquare();
		static Circle* parseCircle();
		static string getVariable(string);
		static Text* parseText();
		static Collidable* parseSlider();
		static Province* parseProvince();
		static Nation* parseNation();
		static Unit* parseUnit();

		static void parseCommon(Moveable*);

		static json::array_t getArray(string);
		static float getFloat(string);
		static int getInt(string);
		static string getString(string, string = "");

		static json level_data;

	public:
		static Level* load_font(string, vector<Moveable*>*, vector<Text*>*, int);
		static map<int, Province*> getProvinceMap();
		static map<int, Nation*> getNationMap();
		static map<int, Unit*> getUnitMap();
};
