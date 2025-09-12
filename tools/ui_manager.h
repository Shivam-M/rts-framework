#pragma once

#include "../assets/colour.h"

#include <map>
#include <string>

using namespace std;

class Province;
class Nation;
class Unit;
class Text;
class Panel;

typedef void (*MappingFunction)(Panel*, void*);

class UIManager {
	private:
		static map<string, Panel*> ui_mappings_;
		static map<string, MappingFunction> method_mappings_;

	public:
		static string get_name(Panel* ui_panel) { return ""; }
		static Panel* get_panel(const string& ui_name) { return ui_mappings_[ui_name]; }

		static void register_panel(const string& ui_name, Panel* ui_panel);
		static void deregister_panel(const string& ui_name);
		static void deregister_panel(Panel* ui_panel);

		static void show(const string& ui_name, const bool& with_fade = false);
		static void show(Panel* panel, const bool& with_fade = false);

		static void hide(const string& ui_name, const bool& with_fade = false);
		static void hide(Panel* panel, const bool& with_fade = false);

		static void toggle(const string& ui_name, const bool& with_fade = false);
		static void toggle(Panel* panel, const bool& with_fade = false);

		static void assign_values(const string& panel_name, void* moveable, MappingFunction mapping_function = nullptr);
		
		static void map_province(Panel* panel, void* moveable);
		static void map_nation(Panel* panel, void* moveable);
		static void map_unit(Panel* panel, void* moveable);
		static void map_war_declaration(Panel* panel, void* war_details);
		static void map_header(Panel* panel, void* header_details);
		static void map_battle(Panel* panel, void* battle_details);
};
