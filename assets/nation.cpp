#include "nation.h"

#include "../assets/province.h"
#include "../assets/unit.h"
#include "../tools/common.h"

using namespace std;

void Nation::add_province(Province* province) {
	provinces_.push_back(province);
	province->nation = this;
}

void Nation::remove_province(Province* province) {
	if (owns_province(province)) {
		if (get_capital() != province) {
			provinces_.erase(remove(provinces_.begin(), provinces_.end(), province), provinces_.end());
		} else if (get_province_count() > 1) {
			provinces_.erase(remove(provinces_.begin(), provinces_.end(), province), provinces_.end());
			set_capital(get_provinces()[0]);
		} else {
			log_t("** NATION TO LOSE CAPITAL **");
		}
	}
}

int Nation::get_army_size() {
	int army_size = 0;
	for (Unit* unit : units_) {
		army_size += unit->amount;
	}
	return army_size;
}

void Nation::add_unit(Unit* unit) {
	units_.push_back(unit);
	unit->nation = this;
	unit->set_colour(colour);
}

void Nation::evaluate() {
	float final_income = income;

	for (Province* province : get_provinces()) {
		province->evaluate();
		final_income += province->value;
	}

	for (Unit* unit : get_army()) {
		unit->evaluate();
	}

	money += final_income - outgoings;
}

Unit* Nation::hire_unit(int size, float skill) {
	float cost = size * UNIT_COST * skill;
	if (money <= cost) return nullptr;
	Unit* unit = new Unit(UnitCount++, this, size, skill, get_capital());
	money -= -cost;
	add_unit(unit);
	return unit;
}

void Nation::dismiss_unit(Unit* unit) {
	units_.erase(remove(units_.begin(), units_.end(), unit), units_.end());
	delete unit;
}