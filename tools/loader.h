#pragma once

#include <json.hpp>

using namespace nlohmann;
using namespace std;

class Province;
class Nation;
class Unit;
class Moveable;
class Circle;
class Text;
class Collidable;
class ParticleGroup;
class Panel;
class Colour;
class Vector4;
struct Vector2;

struct Level {
	vector<Moveable*> objects;
	vector<Text*> text_objects;
	bool offset_positions = true;
};

class Loader {
	private:
		static void parseCommon(Moveable*);
		static ParticleGroup* parseStars(vector<Moveable*>*);
		static Collidable* parseCollidable();
		static Collidable* parseCustom();
		static Moveable* parseMoveable();;
		static string getVariable(string);
		static Text* parseText();
		static Moveable* parseButton();
		static Collidable* parseSlider();
		static Province* parseProvince();
		static Nation* parseNation();
		static Unit* parseUnit();
		static Panel* parsePanel();

		template <typename T>
		static T getValue(string key);
		static bool getBool(string);
		static float getFloat(string);
		static int getInt(string);
		static string getString(string, string = "");
		static json::array_t getArray(string);
		
		// TODO: Come back to this - will need support in level_designer.py too
		/*
		template <typename T>
		static T getJsonValue(string, string);
		static bool getJsonBool(string);
		static float getJsonFloat(string);
		static int getJsonInt(string);
		static string getJsonString(string, string = "");
		*/

		static json current_level_data;

	public:
		static Level* load_level(string, vector<Moveable*>*, vector<Text*>*, int);
		static map<int, Province*>& getProvinceMap();
		static map<int, Nation*>& getNationMap();
		static map<int, Unit*>& getUnitMap();
		static void writeObjects(vector<Moveable*> objects, vector<Text*> text_objects);
};
