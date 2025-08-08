#pragma once

#include <json.hpp>
#include <string>
#include <map>
#include <vector>

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
		static string getVariable(const string&);
		static Text* parseText();
		static Moveable* parseButton();
		static Collidable* parseSlider();
		static Province* parseProvince();
		static Nation* parseNation();
		static Unit* parseUnit();
		static Panel* parsePanel();

		template <typename T>
		static T getValue(const string& key);
		static bool getBool(const string&);
		static float getFloat(const string&);
		static int getInt(const string&);
		static string getString(const string&, const string& = "");
		static json::array_t getArray(const string&);
		
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
		static Level* load_level(string, vector<Moveable*>*, vector<Text*>*, const int&);
		static map<int, Province*>& getProvinceMap();
		static map<int, Nation*>& getNationMap();
		static map<int, Unit*>& getUnitMap();
		static void writeObjects(vector<Moveable*> objects, vector<Text*> text_objects);
};
