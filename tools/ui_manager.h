#pragma once
#include <map>
#include <string>
#include "../assets/panel.h"

using namespace std;

class Province;
class Nation;
class Unit;
class Moveable;
class Text;
class Panel;

struct Mapping {
	Mapping() { content = "", colour = COLOUR_INVIS; };
	Mapping(string _content, Colour _colour = COLOUR_INVIS) { content = _content, colour = _colour; }
	string content = "";
	Colour colour = COLOUR_INVIS;
};

typedef map<string, Mapping>(*MappingFunction)(Moveable*);

class UIManager {
	private:
		static map<string, Panel*> ui_mappings_;
		static map<string, MappingFunction> method_mappings_;

	public:
		static string GetName(Panel* ui_panel) { return ""; }
		static Panel* GetPanel(string ui_name) { return ui_mappings_[ui_name]; }

		static void Register(string ui_name, Panel* ui_panel) { ui_mappings_[ui_name] = ui_panel; }
		static void Unregister(string ui_name) { ui_mappings_.erase(ui_name); }
		static void Unregister(Panel* ui_panel) { Unregister(GetName(ui_panel)); }

		static void Show(string ui_name, bool with_fade = false) { Show(GetPanel(ui_name), with_fade); }
		static void Show(Panel* panel, bool with_fade = false) { panel->show(with_fade); };

		static void Hide(string ui_name, bool with_fade = false) { Hide(GetPanel(ui_name), with_fade); };
		static void Hide(Panel* panel, bool with_fade = false) { panel->hide(with_fade); };

		static void Toggle(string ui_name, bool with_fade = false) { Toggle(GetPanel(ui_name), with_fade); }
		static void Toggle(Panel* panel, bool with_fade = false) { if (panel->hasFlag(DISABLED)) Show(panel, with_fade); else Hide(panel, with_fade); }

		static void AssignValues(const string& panel_name, Moveable* moveable);
		static void UpdateValues(Panel* panel, const map<string, Mapping>& mappings);
		
		static map<string, Mapping> MapProvince(Moveable* moveable);
		static map<string, Mapping> MapNation(Moveable* moveable);
		static map<string, Mapping> MapUnit(Moveable* moveable);
};
