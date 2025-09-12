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
		static void parse_common(Moveable*);
		static ParticleGroup* parse_stars(vector<Moveable*>*);
		static Collidable* parse_collidable();
		static Collidable* parse_custom();
		static Collidable* parse_slider();
		static Moveable* parse_button();
		static Moveable* parse_moveable();;
		static Text* parse_text();
		static Province* parse_province();
		static Nation* parse_nation();
		static Unit* parse_unit();
		static Panel* parse_panel();
		static string get_variable(const string&);

		template <typename T>
		static T get_value(const string& key);
		static bool get_bool(const string&);
		static float get_float(const string&);
		static int get_int(const string&);
		static string get_string(const string&, const string& = "");
		static json::array_t get_array(const string&);
		
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
		static Level* load_level(string file_path, vector<Moveable*>* queue_objects, vector<Text*>* text_objects, const int& identifier, const string& instance_suffix = "");
		static map<int, Province*>& get_province_map();
		static map<int, Nation*>& get_nation_map();
		static map<int, Unit*>& get_unit_map();
		static void write_objects(vector<Moveable*> objects, vector<Text*> text_objects);
};
