#ifndef LOADER_H
#define LOADER_H
#pragma once

// #include <vector>
// #include "../assets/moveable.h"
#include <json.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>

#include "../shapes/circle.h"
#include "../effects/stars.h"
#include "../effects/fire.h"
#include "../assets/collidable.h"
#include "../assets/text.h"


#include "../tools/structs.h"
#include "../tools/common.h"
#include "../tools/fonts.h"

class ProvinceMV;
class Nation;
class Unit;

using namespace nlohmann;
using namespace std;



class Loader {
	private:
		static Fire* parseFire(json);
		static Stars* parseStars(json);
		static Collidable* parseCollidable(json);
		static Collidable* parseQuad(json);
		static Moveable* parseMoveable(json);
		static Square* parseSquare(json);
		static Circle* parseCircle(json);
		static string getVariable(string);
		static Text* parseText(json);
		static Collidable* parseSlider(json);
		static ProvinceMV* parseProvince(json);
		static Nation* parseNation(json);
		static Unit* parseUnit(json);

		static void parseCommon(json, Moveable*);

		static json::array_t getArray(json, string);
		static double getDouble(json, string);
		static int getInt(json, string);
		static string getString(json, string, string = "");

		static json ldata;

	public:
		static Level* load(string, vector<Moveable*>*, vector<Text*>*,  int);
		static map<int, ProvinceMV*> getProvinceMap();
		static map<int, Nation*> getNationMap();
		static map<int, Unit*> getUnitMap();
		

};

#endif
