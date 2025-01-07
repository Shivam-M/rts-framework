#include "ui_manager.h"

#include "../assets/province.h"
#include "../assets/unit.h"
#include "../assets/nation.h"
#include "../game_rts.h"

#include <sstream>

void set_war_details_for_allies(map<string, Mapping>& war_mappings, string side, int strength, int provinces, vector<Nation*> allies);
map<string, Panel*> UIManager::ui_mappings_ = {};

map<string, MappingFunction> UIManager::method_mappings_ = {
    {"ui_province_tooltip",     &UIManager::MapProvince},
    {"ui_nation_tooltip",       &UIManager::MapNation},
    {"ui_unit_tooltip",         &UIManager::MapUnit},
    {"ui_war_declaration",      &UIManager::MapWarDeclaration}
};

void UIManager::AssignValues(const string& panel_name, void* moveable) {
    UpdateValues(UIManager::GetPanel(panel_name), method_mappings_[panel_name](moveable));
}

void UIManager::UpdateValues(Panel* panel, const map<string, Mapping>& mappings) {
    if (!panel) return;
    for (Moveable* panel_item : *panel->get()) {
        if (mappings.count(panel_item->getName())) {
            if (panel_item->hasFlag(TEXT)) {
                if (Text* text_item = dynamic_cast<Text*>(panel_item)) {
                    text_item->setContent(mappings.at(panel_item->getName()).content);
                }
            }
            if (mappings.at(panel_item->getName()).colour != COLOUR_INVIS) {
                panel_item->setColour(mappings.at(panel_item->getName()).colour);
            }
        }
    }
}

map<string, Mapping> UIManager::MapProvince(void* moveable) {
    Province* province = (Province*)moveable;
    map<string, Mapping> province_mappings = {};

    province_mappings["ui_province_tooltip_name"] = Mapping(province->getName(), province->getColour().setW(255));
    province_mappings["ui_province_tooltip_owned_by"] = Mapping("Owned by: " + province->getNation()->getName());

    ostringstream province_value;
    province_value << fixed << setprecision(2) << province->getValue();

    province_mappings["ui_province_tooltip_value"] = Mapping("Value: " + province_value.str());
    province_mappings["ui_province_tooltip_terrain"] = Mapping("Terrain: " + province->getTerrainName());

    if (province->getSiegeProgress() <= 0)
        province_mappings["ui_province_tooltip_siege"] = Mapping("");
    else if (province->getSiegeProgress() < 100)
        province_mappings["ui_province_tooltip_siege"] = Mapping("Siege progress: " + to_string((int)province->getSiegeProgress()) + "%");
    else
        if (province->getController())
            province_mappings["ui_province_tooltip_siege"] = Mapping("Sieged by: " + province->getController()->getNation()->getName());

    return province_mappings;
}

map<string, Mapping> UIManager::MapNation(void* moveable) {
    Nation* nation = (Nation*)moveable;
    map<string, Mapping> nation_mappings = {};

    nation_mappings["ui_nation_tooltip_name"] = Mapping(nation->getName(), nation->getColour().setW(150));
    nation_mappings["ui_nation_tooltip_capital"] = Mapping("Capital: " + nation->getCapital()->getName());

    ostringstream nation_treasury;
    nation_treasury << fixed << setprecision(2) << nation->getMoney();

    nation_mappings["ui_nation_tooltip_treasury"] = Mapping("Treasury: " + nation_treasury.str());
    nation_mappings["ui_nation_tooltip_army_size"] = Mapping("Army Size: " + to_string(nation->getArmySize()));

    return nation_mappings;
}

map<string, Mapping> UIManager::MapUnit(void* moveable) {
    Unit* unit = (Unit*)moveable;
    map<string, Mapping> unit_mappings = {};

    unit_mappings["ui_unit_tooltip_name"] = Mapping(unit->getName(), unit->getColour().setW(255));
    unit_mappings["ui_unit_tooltip_owned_by"] = Mapping("Owned by: " + unit->getNation()->getName());

    ostringstream unit_skill;
    unit_skill << fixed << setprecision(2) << unit->getSkill();

    unit_mappings["ui_unit_tooltip_value"] = Mapping("Skill Rating: " + unit_skill.str());
    unit_mappings["ui_unit_tooltip_terrain"] = Mapping("Size: " + to_string(unit->getAmount()));

    return unit_mappings;
}

map<string, Mapping> UIManager::MapWarDeclaration(void* war_details) {
    War* war = (War*)war_details;
    Nation* attacker_nation = war->attacker;
    Nation* defender_nation = war->defender;
    map<string, Mapping> war_mappings = {};

    war_mappings["ui_war_declaration_attackers_text"] = Mapping(attacker_nation->getName());
    war_mappings["ui_war_declaration_attackers_allies"] = Mapping("None");
    war_mappings["ui_war_declaration_defenders_text"] = Mapping(defender_nation->getName());
    war_mappings["ui_war_declaration_defenders_allies"] = Mapping("None");

    set_war_details_for_allies(war_mappings, "attackers", attacker_nation->getArmySize(), attacker_nation->getOwnedProvinces().size(), war->attacker_allies);
    set_war_details_for_allies(war_mappings, "defenders", defender_nation->getArmySize(), defender_nation->getOwnedProvinces().size(), war->defender_allies);

    switch (war->war_goal) {
        case TAKE_KEY_PROVINCE:
            war_mappings["ui_war_declaration_text_war_goal_details"] = Mapping("Control Province '" + war->war_goal_target.target_province->getName() + "'");
            break;
        case TAKE_MULTIPLE_PROVINCES:
            war_mappings["ui_war_declaration_text_war_goal_details"] = Mapping("Control At Least " + to_string(war->war_goal_target.target_number) + " Provinces");
            break;
        case VASSALISE:
            war_mappings["ui_war_declaration_text_war_goal_details"] = Mapping("Hold Full Control Of '" + defender_nation->getName() + "'");
            break;
        default:
            war_mappings["ui_war_declaration_text_war_goal_details"] = Mapping("Special Objective");
    }

    return war_mappings;
}

void set_war_details_for_allies(map<string, Mapping>& war_mappings, string side, int strength, int provinces, vector<Nation*> allies) {
    stringstream allies_names;
    for (int i = 0; i < allies.size(); i++) {
        strength += allies[i]->getArmySize();
        provinces += allies[i]->getOwnedProvinces().size();
        allies_names << allies[i]->getName();
        if (i < allies.size() - 1) allies_names << ", ";
    }
    if (allies.size() > 0) war_mappings["ui_war_declaration_" + side + "_allies"] = Mapping(allies_names.str());
    war_mappings["ui_war_declaration_" + side + "_strength"] = Mapping(to_string(strength));
    war_mappings["ui_war_declaration_" + side + "_provinces"] = Mapping(to_string(provinces));
}
