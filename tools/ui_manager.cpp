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

// TODO: Expose moveables for direct access instead of using Mapping struct

static map<string, Mapping> province_mappings;
static map<string, Mapping> nation_mappings;
static map<string, Mapping> unit_mappings;
static map<string, Mapping> war_mappings;
static map<string, Mapping> header_mappings;
static map<string, Mapping> battle_mappings;

static string temp_str_buffer;
static ostringstream temp_stream;

map<string, Panel*> UIManager::ui_mappings_ = {};

map<string, MappingFunction> UIManager::method_mappings_ = {
    {"ui_province_tooltip",     &UIManager::MapProvince},
    {"ui_nation_tooltip",       &UIManager::MapNation},
    {"ui_unit_tooltip",         &UIManager::MapUnit},
    {"ui_war_declaration",      &UIManager::MapWarDeclaration},
    {"ui_information_header",   &UIManager::MapHeader},
    {"ui_battle_unit",          &UIManager::MapBattle}
};

static void set_battle_details_for_allies(map<string, Mapping>& battle_mappings, const string& side, const vector<Unit*>& allies);
static void set_war_details_for_allies(map<string, Mapping>& war_mappings, const string& side, int strength, int provinces, const vector<Nation*>& allies);

void UIManager::AssignValues(const string& panel_name, void* moveable) {
    const auto& mappings = method_mappings_[panel_name](moveable);
    UpdateValues(UIManager::GetPanel(panel_name), mappings);
}

void UIManager::UpdateValues(Panel* panel, const map<string, Mapping>& mappings) {
    if (!panel) return;

    const auto& panel_items = *panel->get();
    for (Moveable* panel_item : panel_items) {
        const string& item_name = panel_item->getName();
        auto it = mappings.find(item_name);

        if (it != mappings.end()) {
            const Mapping& mapping = it->second;

            if (panel_item->hasFlag(TEXT)) {
                if (Text* text_item = static_cast<Text*>(panel_item)) {
                    text_item->setContent(mapping.content);
                }
            }
            if (mapping.colour != COLOUR_INVIS) {
                panel_item->setColour(mapping.colour);
            }
        }
    }
}

const map<string, Mapping>& UIManager::MapProvince(void* moveable) {
    Province* province = static_cast<Province*>(moveable);
    Nation* nation = province->getNation();

    province_mappings["ui_province_tooltip_name"] = Mapping(province->getName(), province->getColour().setW(255));
    province_mappings["ui_province_tooltip_owned_by"] = Mapping("Owned by: " + nation->getName());

    const float& value = province->getValue();
    province_mappings["ui_province_tooltip_value"] = Mapping(format("Value: {:.2f}", value));

    province_mappings["ui_province_tooltip_terrain"] = Mapping("Terrain: " + province->getTerrainName());

    const float& siege_progress = province->getSiegeProgress();
    if (siege_progress <= 0) {
        province_mappings["ui_province_tooltip_siege"] = Mapping("");
    } else if (siege_progress < 100) {
        province_mappings["ui_province_tooltip_siege"] = Mapping(format("Siege progress: {}%", static_cast<int>(siege_progress)));
    } else if (province->getController()) {
        province_mappings["ui_province_tooltip_siege"] = Mapping(format("Sieged by: {}", province->getController()->getNation()->getName()));
    }

    return province_mappings;
}

const map<string, Mapping>& UIManager::MapNation(void* moveable) {
    Nation* nation = static_cast<Nation*>(moveable);

    nation_mappings["ui_nation_tooltip_name"] = Mapping(nation->getName(), nation->getColour().setW(150));
    nation_mappings["ui_nation_tooltip_capital"] = Mapping("Capital: " + nation->getCapital()->getName());

    const float& money = nation->getMoney();
    nation_mappings["ui_nation_tooltip_treasury"] = Mapping(format("Treasury: {:.2f}", money));

    const int& army_size = nation->getArmySize();
    nation_mappings["ui_nation_tooltip_army_size"] = Mapping(format("Army Size: {}", army_size));

    return nation_mappings;
}

const map<string, Mapping>& UIManager::MapUnit(void* moveable) {
    Unit* unit = static_cast<Unit*>(moveable);
    Nation* nation = unit->getNation();

    unit_mappings["ui_unit_tooltip_name"] = Mapping(unit->getName(), unit->getColour().setW(255));

    if (nation)
        unit_mappings["ui_unit_tooltip_owned_by"] = Mapping("Owned by: " + nation->getName());
    else
        unit_mappings["ui_unit_tooltip_owned_by"] = Mapping("Independent");  // maybe mercenary's don't need a nation or use a dummy one

    const float& skill = unit->getSkill();
    unit_mappings["ui_unit_tooltip_value"] = Mapping(format("Skill: {:.2f}", skill));

	const int& unit_size = unit->getAmount();
    unit_mappings["ui_unit_tooltip_terrain"] = Mapping(format("Size: {} ", unit_size));

    return unit_mappings;
}

const map<string, Mapping>& UIManager::MapWarDeclaration(void* war_details) {
    War* war = static_cast<War*>(war_details);
    Nation* attacker_nation = war->attacker;
    Nation* defender_nation = war->defender;

    war_mappings["ui_war_declaration_attackers_text"] = Mapping(attacker_nation->getName());
    war_mappings["ui_war_declaration_attackers_allies"] = Mapping("None");
    war_mappings["ui_war_declaration_defenders_text"] = Mapping(defender_nation->getName());
    war_mappings["ui_war_declaration_defenders_allies"] = Mapping("None");

    set_war_details_for_allies(war_mappings, "attackers", attacker_nation->getArmySize(), attacker_nation->getOwnedProvinces().size(), war->attacker_allies);
    set_war_details_for_allies(war_mappings, "defenders", defender_nation->getArmySize(), defender_nation->getOwnedProvinces().size(), war->defender_allies);

    switch (war->war_goal) {
        case TAKE_KEY_PROVINCE:
            war_mappings["ui_war_declaration_text_war_goal_details"] = Mapping(format("Control Province '{}'", war->war_goal_target.target_province->getName()));
            break;
        case TAKE_MULTIPLE_PROVINCES:
            war_mappings["ui_war_declaration_text_war_goal_details"] = Mapping(format("Control At Least {} Provinces", war->war_goal_target.target_number));
            break;
        case VASSALISE:
            war_mappings["ui_war_declaration_text_war_goal_details"] = Mapping(format("Hold Full Control Of ''", defender_nation->getName()));
            break;
        default:
            war_mappings["ui_war_declaration_text_war_goal_details"] = Mapping("Special Objective");
    }

    return war_mappings;
}

const map<string, Mapping>& UIManager::MapHeader(void* header_details) {
    HeaderInformation* information = static_cast<HeaderInformation*>(header_details);

    header_mappings["ui_information_header_gold"] = Mapping(format("${}", information->money));
    header_mappings["ui_information_header_date"] = Mapping(information->date);

    return header_mappings;
}

const map<string, Mapping>& UIManager::MapBattle(void* battle_details) {
    BattleInformation* information = static_cast<BattleInformation*>(battle_details);

    set_battle_details_for_allies(battle_mappings, "1", information->attacker_units);
    set_battle_details_for_allies(battle_mappings, "2", information->defender_units);

    // battle_mappings["ui_battle_unit_power_bar"] = Mapping(information->date);

    return battle_mappings;
}

static void set_battle_details_for_allies(map<string, Mapping>& battle_mappings, const string& side, const vector<Unit*>& allies) {
    int total_amount = 0;
    
    for (Unit* ally: allies) {
        total_amount += ally->getAmount();
    }

    battle_mappings["ui_battle_unit_fighter_" + side] = Mapping(to_string(total_amount));

}

static void set_war_details_for_allies(map<string, Mapping>& war_mappings, const string& side, int strength, int provinces, const vector<Nation*>& allies) {
    temp_stream.str("");
    temp_stream.clear();

    for (size_t i = 0; i < allies.size(); i++) {
        strength += allies[i]->getArmySize();
        provinces += allies[i]->getOwnedProvinces().size();
        temp_stream << allies[i]->getName();
        if (i < allies.size() - 1) temp_stream << ", ";
    }

    if (!allies.empty()) {
        war_mappings["ui_war_declaration_" + side + "_allies"] = Mapping(temp_stream.str());
    }

    temp_str_buffer = to_string(strength);
    war_mappings["ui_war_declaration_" + side + "_strength"] = Mapping(temp_str_buffer);

    temp_str_buffer = to_string(provinces);
    war_mappings["ui_war_declaration_" + side + "_provinces"] = Mapping(temp_str_buffer);
}
