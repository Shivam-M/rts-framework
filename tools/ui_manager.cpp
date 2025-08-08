#include "ui_manager.h"

#include "../game_rts.h"
#include "../assets/province.h"
#include "../assets/unit.h"
#include "../assets/nation.h"
#include "../assets/text.h"
#include "../tools/common.h"

#include <sstream>
#include <map>
#include <string>

using namespace std;

map<string, Panel*> UIManager::ui_mappings_ = {};

map<string, MappingFunction> UIManager::method_mappings_ = {
    {"ui_province_tooltip",     &UIManager::MapProvince},
    {"ui_nation_tooltip",       &UIManager::MapNation},
    {"ui_unit_tooltip",         &UIManager::MapUnit},
    {"ui_war_declaration",      &UIManager::MapWarDeclaration},
    {"ui_information_header",   &UIManager::MapHeader},
    {"ui_battle_unit",          &UIManager::MapBattle}
};

static void set_battle_details_for_allies(Panel* panel, const string& side, const vector<Unit*>& allies);
static void set_war_details_for_allies(Panel* panel, const string& side, int strength, int provinces, const vector<Nation*>& allies);

void UIManager::AssignValues(const string& panel_name, void* moveable) {
    method_mappings_[panel_name](UIManager::GetPanel(panel_name), moveable);
}

void UIManager::MapProvince(Panel* panel, void* moveable) { // TODO: check if converting non-ref to float& actually helps
    Province* province = static_cast<Province*>(moveable);
    Nation* nation = province->getNation();

	panel->getTextByName("ui_province_tooltip_name")->setContent(province->getName());
	panel->getTextByName("ui_province_tooltip_name")->setColour(province->getColour().setW(255));
    panel->getTextByName("ui_province_tooltip_owned_by")->setContent("Owned by: " + nation->getName());
    panel->getTextByName("ui_province_tooltip_value")->setContent(format("Value: {:.2f}", province->getValue()));
    panel->getTextByName("ui_province_tooltip_terrain")->setContent("Terrain: " + province->getTerrainName());

    const float& siege_progress = province->getSiegeProgress();
    if (siege_progress <= 0) {
		panel->getTextByName("ui_province_tooltip_siege")->setContent("");
    } else if (siege_progress < 100) {
        panel->getTextByName("ui_province_tooltip_siege")->setContent(format("Siege progress: {}%", static_cast<int>(siege_progress)));
    } else if (province->getController()) {
        panel->getTextByName("ui_province_tooltip_siege")->setContent(format("Sieged by: {}", province->getController()->getNation()->getName()));
    }
}

void UIManager::MapNation(Panel* panel, void* moveable) {
    Nation* nation = static_cast<Nation*>(moveable);

    panel->getTextByName("ui_nation_tooltip_name")->setContent(nation->getName());
    panel->getTextByName("ui_nation_tooltip_name")->setColour(nation->getColour().setW(150));
    panel->getTextByName("ui_nation_tooltip_capital")->setContent("Capital: " + nation->getCapital()->getName());
    panel->getTextByName("ui_nation_tooltip_treasury")->setContent(format("Treasury: {:.2f}", nation->getMoney()));
    panel->getTextByName("ui_nation_tooltip_army_size")->setContent(format("Army Size: {}", static_cast<int>(nation->getOwnedUnits().size())));
}

void UIManager::MapUnit(Panel* panel, void* moveable) {
    Unit* unit = static_cast<Unit*>(moveable);
    Nation* nation = unit->getNation();

	panel->getTextByName("ui_unit_tooltip_name")->setContent(unit->getName());
	panel->getTextByName("ui_unit_tooltip_name")->setColour(unit->getColour().setW(255));
	panel->getTextByName("ui_unit_tooltip_owned_by")->setContent("Owned by: " + (nation ? nation->getName() : "Independent"));  
	panel->getTextByName("ui_unit_tooltip_value")->setContent(format("Skill: {:.2f}", unit->getSkill()));
	panel->getTextByName("ui_unit_tooltip_terrain")->setContent(format("Size: {}", unit->getAmount()));
}

void UIManager::MapWarDeclaration(Panel* panel, void* war_details) {
    War* war = static_cast<War*>(war_details);
    Nation* attacker_nation = war->attacker;
    Nation* defender_nation = war->defender;

	panel->getTextByName("ui_war_declaration_attackers_text")->setContent(attacker_nation->getName());
    panel->getTextByName("ui_war_declaration_attackers_allies")->setContent("None");
	panel->getTextByName("ui_war_declaration_defenders_text")->setContent(defender_nation->getName());
    panel->getTextByName("ui_war_declaration_defenders_allies")->setContent("None");
	panel->getTextByName("ui_war_declaration_text_war_goal_details")->setContent("War Goal: " + war->war_goal_target.target_province->getName());

    set_war_details_for_allies(panel, "attackers", attacker_nation->getArmySize(), attacker_nation->getOwnedProvinces().size(), war->attacker_allies);
    set_war_details_for_allies(panel, "defenders", defender_nation->getArmySize(), defender_nation->getOwnedProvinces().size(), war->defender_allies);

    switch (war->war_goal) {
        case TAKE_KEY_PROVINCE:
            panel->getTextByName("ui_war_declaration_text_war_goal_details")->setContent(format("Control Province '{}'", war->war_goal_target.target_province->getName()));
            break;
        case TAKE_MULTIPLE_PROVINCES:
            panel->getTextByName("ui_war_declaration_text_war_goal_details")->setContent(format("Control At Least {} Provinces", war->war_goal_target.target_number));
            break;
        case VASSALISE:
            panel->getTextByName("ui_war_declaration_text_war_goal_details")->setContent(format("Hold Full Control Of ''", defender_nation->getName()));
            break;
        default:
            panel->getTextByName("ui_war_declaration_text_war_goal_details")->setContent("Special Objective");
    }
}

void UIManager::MapHeader(Panel* panel, void* header_details) {
    HeaderInformation* information = static_cast<HeaderInformation*>(header_details);

	panel->getTextByName("ui_information_header_gold")->setContent(format("${:.2f}", information->money));
	panel->getTextByName("ui_information_header_date")->setContent(information->date);
}

void UIManager::MapBattle(Panel* panel, void* battle_details) {
    BattleInformation* information = static_cast<BattleInformation*>(battle_details);

    set_battle_details_for_allies(panel, "1", information->attacker_units);
    set_battle_details_for_allies(panel, "2", information->defender_units);

    // battle_mappings["ui_battle_unit_power_bar"] = Mapping("?");
}

static void set_battle_details_for_allies(Panel* panel, const string& side, const vector<Unit*>& allies) {
    int total_amount = 0;
    
    for (Unit* ally: allies) {
        total_amount += ally->getAmount();
    }

    panel->getTextByName("ui_battle_unit_fighter_" + side)->setContent(to_string(total_amount));
}

static void set_war_details_for_allies(Panel* panel, const string& side, int strength, int provinces, const vector<Nation*>& allies) {
    ostringstream allies_stream;
    for (size_t i = 0; i < allies.size(); ++i) {
        strength += allies[i]->getArmySize();
        provinces += allies[i]->getOwnedProvinces().size();
        allies_stream << allies[i]->getName();
        if (i + 1 < allies.size()) allies_stream << ", ";
    }

    if (!allies.empty()) {
        panel->getTextByName("ui_war_declaration_" + side + "_allies")->setContent(allies_stream.str());
    }

    panel->getTextByName("ui_war_declaration_" + side + "_strength")->setContent(to_string(strength));
    panel->getTextByName("ui_war_declaration_" + side + "_provinces")->setContent(to_string(provinces));
}
