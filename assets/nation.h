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

enum NATION_FLAGS {AT_WAR, LOSING_MONEY, IN_DEBT, UNLANDED};

struct EventMessage {
	enum EVENT { GENERIC, MESSAGE, BATTLE_START, BATTLE_WON, BATTLE_LOST, UNIT_LOST, REGISTER_UNIT }; // register unit with game
	int event_type = GENERIC;
	string content = "";
	vector<Moveable*> involved;
};

class Nation : public Moveable {
	private:
		int identifier_ = -1;
		string name_ = "Generic Nation";
		Colour colour_ = Colour(255, 255, 255);
		Province* capital_province_;
		vector<Province*> owned_provinces_;
		vector<Unit*> owned_army_units_;
		float money_ = 1.00f;
		float income_ = 0.00f;
		float outgoings_ = 0.00f;
		bool in_debt_ = false;
		int flags = 0;

	public:
		Nation(int identifier, string name, Province* capital = nullptr) : identifier_(identifier), name_(name), capital_province_(capital) { add_flag(NATION); }

		void evaluate();

		void set_id(int identifier) { identifier_ = identifier; }
		int get_id() { return identifier_; }

		Province* get_capital() { return capital_province_; }
		void set_capital(Province* province) { if (owns_province(province)) capital_province_ = province; }

		vector<Province*>& get_provinces() { return owned_provinces_; }
		int get_province_count() { return owned_provinces_.size(); }
		bool owns_province(Province* province) { return find(owned_provinces_.begin(), owned_provinces_.end(), province) != owned_provinces_.end(); }
		void add_province(Province* province);
		void remove_province(Province* province);

		int get_army_size();
		vector<Unit*>& get_army() { return owned_army_units_; }
		void add_unit(Unit* unit);

		float get_money() { return money_; }
		void set_money(float money) { money_ = money; }
		void add_money(float amount) { money_ += amount; }

		float get_income() { return income_; }
		void set_income(float income) { income_ = income; }
		void add_income(float amount) { income_ += amount; }

		float get_expenses() { return outgoings_; }
		void set_expenses(float outgoings) { outgoings_ = outgoings; }
		void add_expense(float amount) { outgoings_ += amount; }

		bool is_in_debt() { return in_debt_; }

		Unit* hire_unit(int size, float skill = 1.00f);

		void dismiss_unit(Unit* unit);
};
