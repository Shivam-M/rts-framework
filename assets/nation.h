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
		Nation(int identifier, string name, Province* capital = nullptr) : identifier_(identifier), name_(name), capital_province_(capital) { addFlag(NATION); }

		void setID(int identifier) { identifier_ = identifier; }
		int getID() { return identifier_; }

		Province* getCapital() { return capital_province_; }
		void setCapital(Province* province) { if (ownsProvince(province)) capital_province_ = province; }

		vector<Province*>& getOwnedProvinces() { return owned_provinces_; }
		int getNumberProvinces() { return owned_provinces_.size(); }
		bool ownsProvince(Province* province) { return find(owned_provinces_.begin(), owned_provinces_.end(), province) != owned_provinces_.end(); }
		void addProvince(Province* province);
		void removeProvince(Province* province);

		int getArmySize();
		vector<Unit*>& getOwnedUnits() { return owned_army_units_; }
		void addUnit(Unit* unit);

		float getMoney() { return money_; }
		void setMoney(float money) { money_ = money; }
		void addMoney(float amount) { money_ += amount; }

		float getIncome() { return income_; }
		void setIncome(float income) { income_ = income; }
		void addIncome(float amount) { income_ += amount; }

		float getOutgoings() { return outgoings_; }
		void setOutgoings(float outgoings) { outgoings_ = outgoings; }
		void addOutgoing(float amount) { outgoings_ += amount; }

		bool isInDebt() { return in_debt_; }

		void evaluate();

		Unit* hireUnit(int size, float skill = 1.00f);

		void dismissUnit(Unit* unit);
};
