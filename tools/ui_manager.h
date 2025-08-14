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
		static string GetName(Panel* ui_panel) { return ""; }
		static Panel* GetPanel(const string& ui_name) { return ui_mappings_[ui_name]; }

		static void Register(const string& ui_name, Panel* ui_panel);
		static void Unregister(const string& ui_name);
		static void Unregister(Panel* ui_panel);

		static void Show(const string& ui_name, const bool& with_fade = false);
		static void Show(Panel* panel, const bool& with_fade = false);

		static void Hide(const string& ui_name, const bool& with_fade = false);
		static void Hide(Panel* panel, const bool& with_fade = false);

		static void Toggle(const string& ui_name, const bool& with_fade = false);
		static void Toggle(Panel* panel, const bool& with_fade = false);

		static void AssignValues(const string& panel_name, void* moveable, MappingFunction mapping_function = nullptr);
		
		static void MapProvince(Panel* panel, void* moveable);
		static void MapNation(Panel* panel, void* moveable);
		static void MapUnit(Panel* panel, void* moveable);
		static void MapWarDeclaration(Panel* panel, void* war_details);
		static void MapHeader(Panel* panel, void* header_details);
		static void MapBattle(Panel* panel, void* battle_details);
};
