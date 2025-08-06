#include "nation.h"

#include "../assets/province.h"
#include "../assets/unit.h"
#include "../tools/common.h"

using namespace std;

void Nation::addProvince(Province* province) {
	owned_provinces_.push_back(province);
	province->setNation(this);
}

void Nation::removeProvince(Province* province) {
	if (ownsProvince(province)) {
		if (getCapital() != province) {
			owned_provinces_.erase(remove(owned_provinces_.begin(), owned_provinces_.end(), province), owned_provinces_.end());
		} else if (getNumberProvinces() > 1) {
			owned_provinces_.erase(remove(owned_provinces_.begin(), owned_provinces_.end(), province), owned_provinces_.end());
			setCapital(getOwnedProvinces()[0]);
		} else {
			log_t("** NATION TO LOSE CAPITAL **");
		}
	}
}

int Nation::getArmySize() {
	int army_size = 0;
	for (Unit* unit : owned_army_units_) {
		army_size += unit->getAmount();
	}
	return army_size;
}

void Nation::addUnit(Unit* unit) {
	owned_army_units_.push_back(unit);
	unit->setNation(this);
	unit->setColour(getColour());
}

void Nation::evaluate() {
	float final_income = income_;

	for (Province* province : getOwnedProvinces()) {
		province->evaluate();
		final_income += province->getValue();
	}

	for (Unit* unit : getOwnedUnits()) {
		unit->evaluate();
	}

	money_ += final_income - getOutgoings();
	in_debt_ = money_ < 0.00f;
}

Unit* Nation::hireUnit(int size, float skill) {
	float cost = size * UNIT_COST * skill;
	if (money_ <= cost) return nullptr;
	Unit* unit = new Unit(UnitCount++, this, size, skill, getCapital());
	addMoney(-cost);
	addUnit(unit);
	return unit;
}

void Nation::dismissUnit(Unit* unit) {
	owned_army_units_.erase(remove(owned_army_units_.begin(), owned_army_units_.end(), unit), owned_army_units_.end());
	delete unit;
}