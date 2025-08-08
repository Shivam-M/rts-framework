#pragma once

#include "../assets/panel.h"
#include "../assets/colour.h"

#include <map>
#include <string>

using namespace std;

class Province;
class Nation;
class Unit;
class Text;

typedef void (*MappingFunction)(Panel*, void*);

class UIManager {
	private:
		static map<string, Panel*> ui_mappings_;
		static map<string, MappingFunction> method_mappings_;

	public:
		static string GetName(Panel* ui_panel) { return ""; }
		static Panel* GetPanel(const string& ui_name) { return ui_mappings_[ui_name]; }

		static void Register(string ui_name, Panel* ui_panel) { ui_mappings_[ui_name] = ui_panel; }
		static void Unregister(string ui_name) { ui_mappings_.erase(ui_name); }
		static void Unregister(Panel* ui_panel) { Unregister(GetName(ui_panel)); }

		static void Show(const string& ui_name, bool with_fade = false) { Show(GetPanel(ui_name), with_fade); }
		static void Show(Panel* panel, bool with_fade = false) { panel->show(with_fade); };

		static void Hide(const string& ui_name, bool with_fade = false) { Hide(GetPanel(ui_name), with_fade); };
		static void Hide(Panel* panel, bool with_fade = false) { panel->hide(with_fade); };

		static void Toggle(const string& ui_name, bool with_fade = false) { Toggle(GetPanel(ui_name), with_fade); }
		static void Toggle(Panel* panel, bool with_fade = false) { if (panel->hasFlag(DISABLED)) Show(panel, with_fade); else Hide(panel, with_fade); }

		static void AssignValues(const string& panel_name, void* moveable);
		
		static void MapProvince(Panel* panel, void* moveable);
		static void MapNation(Panel* panel, void* moveable);
		static void MapUnit(Panel* panel, void* moveable);
		static void MapWarDeclaration(Panel* panel, void* war_details);
		static void MapHeader(Panel* panel, void* header_details);
		static void MapBattle(Panel* panel, void* battle_details);
};
