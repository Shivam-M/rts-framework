#include "ui_manager.h"

#include "../assets/province.h"
#include "../assets/unit.h"
#include "../assets/nation.h"

map<string, Panel*> UIManager::ui_mappings_ = {};

map<string, MappingFunction> UIManager::method_mappings_ = {
    {"ui_province_tooltip", &UIManager::MapProvince},
    {"ui_nation_tooltip", &UIManager::MapNation},
    {"ui_unit_tooltip", &UIManager::MapUnit},
};

void UIManager::AssignValues(const string& panel_name, Moveable* moveable) {
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

map<string, Mapping> UIManager::MapProvince(Moveable* moveable) {
    Province* province = dynamic_cast<Province*>(moveable);
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

map<string, Mapping> UIManager::MapNation(Moveable* moveable) {
    Nation* nation = dynamic_cast<Nation*>(moveable);
    map<string, Mapping> nation_mappings = {};

    nation_mappings["ui_nation_tooltip_name"] = Mapping(nation->getName(), nation->getColour().setW(150));
    nation_mappings["ui_nation_tooltip_capital"] = Mapping("Capital: " + nation->getCapital()->getName());

    ostringstream nation_treasury;
    nation_treasury << fixed << setprecision(2) << nation->getMoney();

    nation_mappings["ui_nation_tooltip_treasury"] = Mapping("Treasury: " + nation_treasury.str());
    nation_mappings["ui_nation_tooltip_army_size"] = Mapping("Army Size: " + to_string(nation->getArmySize()));

    return nation_mappings;
}

map<string, Mapping> UIManager::MapUnit(Moveable* moveable) {
    Unit* unit = dynamic_cast<Unit*>(moveable);
    map<string, Mapping> unit_mappings = {};

    unit_mappings["ui_unit_tooltip_name"] = Mapping(unit->getName(), unit->getColour().setW(255));
    unit_mappings["ui_unit_tooltip_owned_by"] = Mapping("Owned by: " + unit->getNation()->getName());

    ostringstream unit_skill;
    unit_skill << fixed << setprecision(2) << unit->getSkill();

    unit_mappings["ui_unit_tooltip_value"] = Mapping("Skill Rating: " + unit_skill.str());
    unit_mappings["ui_unit_tooltip_terrain"] = Mapping("Size: " + to_string(unit->getAmount()));

    return unit_mappings;
}