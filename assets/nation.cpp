#include "nation.h"

#include "../assets/province.h"
#include "../assets/unit.h"
#include "../tools/common.h"

using namespace std;

void Nation::add_province(Province* province) {
	owned_provinces_.push_back(province);
	province->set_nation(this);
}

void Nation::remove_province(Province* province) {
	if (owns_province(province)) {
		if (get_capital() != province) {
			owned_provinces_.erase(remove(owned_provinces_.begin(), owned_provinces_.end(), province), owned_provinces_.end());
		} else if (get_province_count() > 1) {
			owned_provinces_.erase(remove(owned_provinces_.begin(), owned_provinces_.end(), province), owned_provinces_.end());
			set_capital(get_provinces()[0]);
		} else {
			log_t("** NATION TO LOSE CAPITAL **");
		}
	}
}

int Nation::get_army_size() {
	int army_size = 0;
	for (Unit* unit : owned_army_units_) {
		army_size += unit->get_amount();
	}
	return army_size;
}

void Nation::add_unit(Unit* unit) {
	owned_army_units_.push_back(unit);
	unit->set_nation(this);
	unit->set_colour(get_colour());
}

void Nation::evaluate() {
	float final_income = income_;

	for (Province* province : get_provinces()) {
		province->evaluate();
		final_income += province->get_value();
	}

	for (Unit* unit : get_army()) {
		unit->evaluate();
	}

	money_ += final_income - get_expenses();
	in_debt_ = money_ < 0.00f;
}

Unit* Nation::hire_unit(int size, float skill) {
	float cost = size * UNIT_COST * skill;
	if (money_ <= cost) return nullptr;
	Unit* unit = new Unit(UnitCount++, this, size, skill, get_capital());
	add_money(-cost);
	add_unit(unit);
	return unit;
}

void Nation::dismiss_unit(Unit* unit) {
	owned_army_units_.erase(remove(owned_army_units_.begin(), owned_army_units_.end(), unit), owned_army_units_.end());
	delete unit;
}