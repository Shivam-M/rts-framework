#pragma once

#include "moveable.h"

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Province;
class Unit;

static float UNIT_COST = 0.02f;
static int UnitCount = 1000;

class Nation : public Moveable {
	enum NATION_FLAGS : uint8_t { AT_WAR, LOSING_MONEY, IN_DEBT, UNLANDED };

	private:
		Province* capital_;
		vector<Province*> provinces_;
		vector<Unit*> units_;

	public:
		int identifier = -1;
		float money = 1.00f;
		float income = 0.00f;
		float outgoings = 0.00f;

		virtual ~Nation() {};
		Nation(int id, Province* capital = nullptr) : identifier(id), capital_(capital) { add_flag(NATION); }

		void evaluate();
		void add_unit(Unit* unit);
		void dismiss_unit(Unit* unit);
		int get_army_size();
		const vector<Unit*>& get_army() { return units_; }
		Unit* hire_unit(int size, float skill = 1.00f);

		Province* get_capital() { return capital_; }
		void set_capital(Province* province) { if (owns_province(province)) capital_ = province; }

		vector<Province*>& get_provinces() { return provinces_; }
		int get_province_count() { return provinces_.size(); }
		bool owns_province(Province* province) { return find(provinces_.begin(), provinces_.end(), province) != provinces_.end(); }
		void add_province(Province* province);
		void remove_province(Province* province);
};
