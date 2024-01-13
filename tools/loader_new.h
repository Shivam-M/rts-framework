#pragma once

#include <json.hpp>

using namespace nlohmann;
using namespace std;

#include "../defs/colour.h"
#include "../tools/fonts.h"

class ProvinceNew;
class NationNew;
class UnitNew;
class MoveableNew;
class CircleNew;
class TextNew;
class CollidableNew;

struct Level2 {
	Vector2 player_position;
	vector<MoveableNew*> objects;
	vector<TextNew*> text_objects;
};

class LoaderNew {
	private:
		// static Fire* parseFire();
		// static Stars* parseStars();
		static CollidableNew* parseCollidable();
		static CollidableNew* parseCustom();
		static MoveableNew* parseMoveable();
		// static Square* parseSquare();
		static CircleNew* parseCircle();
		static string getVariable(string);
		static TextNew* parseText();
		static CollidableNew* parseSlider();
		static ProvinceNew* parseProvince();
		static NationNew* parseNation();
		static UnitNew* parseUnit();

		static void parseCommon(MoveableNew*);

		static json::array_t getArray(string);
		static float getFloat(string);
		static int getInt(string);
		static string getString(string, string = "");

		static json ldata;

	public:
		static Level2* load(string, vector<MoveableNew*>*, vector<TextNew*>*, int);
		static map<int, ProvinceNew*> getProvinceMap();
		static map<int, NationNew*> getNationMap();
		static map<int, UnitNew*> getUnitMap();
};
