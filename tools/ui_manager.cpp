#include "ui_manager.h"

#include "../game_rts.h"
#include "../assets/province.h"
#include "../assets/unit.h"
#include "../assets/nation.h"
#include "../assets/text.h"
#include "../assets/panel.h"
#include "../tools/common.h"

#include <sstream>
#include <map>
#include <string>

using namespace std;

map<string, Panel*> UIManager::ui_mappings_ = {};

map<string, MappingFunction> UIManager::method_mappings_ = {
    {"ui_province_tooltip",     &UIManager::map_province},
    {"ui_nation_tooltip",       &UIManager::map_nation},
    {"ui_unit_tooltip",         &UIManager::map_unit},
    {"ui_war_declaration",      &UIManager::map_war_declaration},
    {"ui_information_header",   &UIManager::map_header},
    {"ui_battle_unit",          &UIManager::map_battle}
};

void set_battle_details_for_allies(Panel* panel, const string& side, const vector<Unit*>& allies, float starting_strength);
void set_war_details_for_allies(Panel* panel, const string& side, int strength, int provinces, const vector<Nation*>& allies);

void UIManager::register_panel(const string& ui_name, Panel* ui_panel) {
    ui_mappings_[ui_name] = ui_panel;
}

void UIManager::deregister_panel(const string& ui_name) {
    ui_mappings_.erase(ui_name);
}

void UIManager::deregister_panel(Panel* ui_panel) {
    deregister_panel(get_name(ui_panel));
}

void UIManager::show(const string& ui_name, const bool& with_fade) {
    show(get_panel(ui_name), with_fade);
}

void UIManager::show(Panel* panel, const bool& with_fade) {
    if (panel) {
        panel->show(with_fade);
    } else {
        log_t(CON_RED "ERROR! Tried to show a panel that does not exist");
    }
}

void UIManager::hide(const string& ui_name, const bool& with_fade) {
    hide(get_panel(ui_name), with_fade);
}

void UIManager::hide(Panel* panel, const bool& with_fade) {
    if (panel) {
        panel->hide(with_fade);
    } else {
        log_t(CON_RED "ERROR! Tried to hide a panel that does not exist");
    }
}

void UIManager::toggle(const string& ui_name, const bool& with_fade) {
    toggle(get_panel(ui_name), with_fade);
}

void UIManager::toggle(Panel* panel, const bool& with_fade) {
    if (!panel) {
        log_t(CON_RED "ERROR! Tried to toggle a panel that does not exist");
        return;
    }
    if (panel->has_flag(DISABLED)) {
        show(panel, with_fade);
    } else {
        hide(panel, with_fade);
    }
}

void UIManager::assign_values(const string& panel_name, void* moveable, MappingFunction mapping_function) {  // temp workaround
    if (!mapping_function) {
		mapping_function = method_mappings_[panel_name];
    }
    mapping_function(UIManager::get_panel(panel_name), moveable);
}

void UIManager::map_province(Panel* panel, void* moveable) { // TODO: check if converting non-ref to float& actually helps
    Province* province = static_cast<Province*>(moveable);
    Nation* nation = province->nation;

	panel->get_text_by_name("ui_province_tooltip_name")->set_content(province->name);
	panel->get_text_by_name("ui_province_tooltip_name")->set_colour(province->colour.with_alpha(255));
    panel->get_text_by_name("ui_province_tooltip_owned_by")->set_content("Owned by: " + nation->name);
    panel->get_text_by_name("ui_province_tooltip_value")->set_content(format("Value: {:.2f}", province->value));
    panel->get_text_by_name("ui_province_tooltip_terrain")->set_content("Terrain: " + province->get_terrain_name());

    const float& siege_progress = province->siege_progress;
    if (siege_progress <= 0) {
		panel->get_text_by_name("ui_province_tooltip_siege")->set_content("");
    } else if (siege_progress < 100) {
        panel->get_text_by_name("ui_province_tooltip_siege")->set_content(format("Siege progress: {}%", static_cast<int>(siege_progress)));
    } else if (province->controller) {
        panel->get_text_by_name("ui_province_tooltip_siege")->set_content(format("Sieged by: {}", province->controller->nation->name));
    }
}

void UIManager::map_nation(Panel* panel, void* moveable) {
    Nation* nation = static_cast<Nation*>(moveable);

    panel->get_text_by_name("ui_nation_tooltip_name")->set_content(nation->name);
    panel->get_text_by_name("ui_nation_tooltip_name")->set_colour(nation->colour.with_alpha(150));
    panel->get_text_by_name("ui_nation_tooltip_capital")->set_content("Capital: " + nation->get_capital()->name);
    panel->get_text_by_name("ui_nation_tooltip_treasury")->set_content(format("Treasury: {:.2f}", nation->money));
    panel->get_text_by_name("ui_nation_tooltip_army_size")->set_content(format("Army Size: {}", static_cast<int>(nation->get_army().size())));
}

void UIManager::map_unit(Panel* panel, void* moveable) {
    Unit* unit = static_cast<Unit*>(moveable);
    Nation* nation = unit->nation;

	panel->get_text_by_name("ui_unit_tooltip_name")->set_content(unit->name);
	panel->get_text_by_name("ui_unit_tooltip_name")->set_colour(unit->colour.with_alpha(255));
	panel->get_text_by_name("ui_unit_tooltip_owned_by")->set_content("Owned by: " + (nation ? nation->name : "Independent"));  
	panel->get_text_by_name("ui_unit_tooltip_value")->set_content(format("Skill: {:.2f}", unit->skill));
	panel->get_text_by_name("ui_unit_tooltip_terrain")->set_content(format("Size: {}", unit->amount));
}

void UIManager::map_war_declaration(Panel* panel, void* war_details) {
    War* war = static_cast<War*>(war_details);
    Nation* attacker_nation = war->attacker;
    Nation* defender_nation = war->defender;

    panel->get_moveable_by_name("ui_war_declaration_background")->set_colour(attacker_nation->colour * Colour(0.25, 0.25, 0.25, 1.25));
    panel->get_moveable_by_name("ui_war_declaration_background")->set_gradient_colour(defender_nation->colour * Colour(0.25, 0.25, 0.25, 1.25));
	panel->get_text_by_name("ui_war_declaration_attackers_text")->set_content(attacker_nation->name);
    panel->get_text_by_name("ui_war_declaration_attackers_allies")->set_content("None");
	panel->get_text_by_name("ui_war_declaration_defenders_text")->set_content(defender_nation->name);
    panel->get_text_by_name("ui_war_declaration_defenders_allies")->set_content("None");
	panel->get_text_by_name("ui_war_declaration_text_war_goal_details")->set_content("War Goal: " + war->war_goal_target.target_province->name);

    set_war_details_for_allies(panel, "attackers", attacker_nation->get_army_size(), attacker_nation->get_provinces().size(), war->attacker_allies);
    set_war_details_for_allies(panel, "defenders", defender_nation->get_army_size(), defender_nation->get_provinces().size(), war->defender_allies);

    switch (war->war_goal) {
        case TAKE_KEY_PROVINCE:
            panel->get_text_by_name("ui_war_declaration_text_war_goal_details")->set_content(format("Control Province '{}'", war->war_goal_target.target_province->name));
            break;
        case TAKE_MULTIPLE_PROVINCES:
            panel->get_text_by_name("ui_war_declaration_text_war_goal_details")->set_content(format("Control At Least {} Provinces", war->war_goal_target.target_number));
            break;
        case VASSALISE:
            panel->get_text_by_name("ui_war_declaration_text_war_goal_details")->set_content(format("Hold Full Control Of ''", defender_nation->name));
            break;
        default:
            panel->get_text_by_name("ui_war_declaration_text_war_goal_details")->set_content("Special Objective");
    }
}

void UIManager::map_header(Panel* panel, void* header_details) {
    HeaderInformation* information = static_cast<HeaderInformation*>(header_details);

	panel->get_text_by_name("ui_information_header_gold")->set_content(format("${:.2f}", information->money));
	panel->get_text_by_name("ui_information_header_date")->set_content(information->date);
}

void UIManager::map_battle(Panel* panel, void* battle_details) {
    BattleInformation* information = static_cast<BattleInformation*>(battle_details);

    set_battle_details_for_allies(panel, "1", information->attacker_units, information->total_attacker_starting_strength);
    set_battle_details_for_allies(panel, "2", information->defender_units, information->total_defender_starting_strength);
}

void set_battle_details_for_allies(Panel* panel, const string& side, const vector<Unit*>& allies, float starting_strength) {
    int total_amount = 0;
    
    for (Unit* ally: allies) {
        total_amount += ally->amount;
    }

    float const MAX_BAR_WIDTH = 0.1f;
    float const BAR_HEIGHT = 0.012f;
    float ratio = total_amount / starting_strength;
    float width = MAX_BAR_WIDTH * ratio;
    float x_position_offset = side == "1" ? -width : 0;
    
    panel->get_text_by_name("ui_battle_unit_fighter_" + side)->set_content(to_string(total_amount));
    panel->get_moveable_by_name("ui_battle_unit_power_bar_" + side)->location.x = panel->get_centre().x + x_position_offset;
    panel->get_moveable_by_name("ui_battle_unit_power_bar_" + side)->set_size(width, BAR_HEIGHT);
    panel->get_moveable_by_name("ui_battle_unit_power_bar_" + side)->set_colour(allies[0]->colour);
    panel->get_moveable_by_name("ui_battle_unit_power_bar_" + side)->set_gradient_colour(allies[0]->colour);
}

void set_war_details_for_allies(Panel* panel, const string& side, int strength, int provinces, const vector<Nation*>& allies) {
    ostringstream allies_stream;
    for (size_t i = 0; i < allies.size(); ++i) {
        strength += allies[i]->get_army_size();
        provinces += allies[i]->get_provinces().size();
        allies_stream << allies[i]->name;
        if (i + 1 < allies.size()) allies_stream << ", ";
    }

    if (!allies.empty()) {
        panel->get_text_by_name("ui_war_declaration_" + side + "_allies")->set_content(allies_stream.str());
    }

    panel->get_text_by_name("ui_war_declaration_" + side + "_strength")->set_content(to_string(strength));
    panel->get_text_by_name("ui_war_declaration_" + side + "_provinces")->set_content(to_string(provinces));
}
