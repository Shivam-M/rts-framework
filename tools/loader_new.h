#pragma once

// #include <vector>
// #include "../assets/moveable.h"
#include <json.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>

#include "../shapes/circle_new.h"
#include "../effects/stars.h"
#include "../effects/fire.h"
#include "../assets/collidable_new.h"

#include "../tools/structs.h"
#include "../tools/common.h"
#include "../tools/fonts.h"

using namespace nlohmann;
using namespace std;

class ProvinceNew;
class NationNew;
class UnitNew;
class MoveableNew;
class CircleNew;
class TextNew;

struct Level2 {
	Vector2 player_position;
	vector<MoveableNew*> objects;
	vector<TextNew*> text_objects;
};

class LoaderNew {
	private:
		static Fire* parseFire();
		static Stars* parseStars();
		static CollidableNew* parseCollidable();
		static CollidableNew* parseCustom();
		static MoveableNew* parseMoveable();
		static Square* parseSquare();
		static CircleNew* parseCircle();
		static string getVariable(string);
		static TextNew* parseText();
		static CollidableNew* parseSlider();
		static ProvinceNew* parseProvince();
		static NationNew* parseNation();
		static UnitNew* parseUnit();

		static void parseCommon(MoveableNew*);

		static json::array_t getArray(string);
		static double getDouble(string);
		static int getInt(string);
		static string getString(string, string = "");

		static json ldata;

	public:
		static Level2* load(string, vector<MoveableNew*>*, vector<TextNew*>*, int);
		static map<int, ProvinceNew*> getProvinceMap();
		static map<int, NationNew*> getNationMap();
		static map<int, UnitNew*> getUnitMap();
};