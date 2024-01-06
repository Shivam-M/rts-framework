#pragma once
#include <vector>

static double UNIT_COST = 0.02;
static int UnitCount = 0;

enum NATION_FLAGS {
	AT_WAR,
	LOSING_MONEY,
	IN_DEBT,
	UNLANDED,
};

class Nation : public Moveable {
	private:
		int identifier_ = -1;
		string name_ = "Generic Nation";
		Colour2 colour_ = Colour2(255, 255, 255);
		ProvinceMV* capital_province_;
		vector<ProvinceMV*> owned_provinces_;
		vector<Unit*> owned_army_units_;
		double money_ = 1.00;
		double income_ = 0.00;
		double outgoings_ = 0.00;
		bool in_debt_ = false;
		int flags = 0;

	public:
		Nation(int identifier, string name, ProvinceMV* capital = nullptr) : identifier_(identifier), name_(name), capital_province_(capital) {}

		void setID(int identifier) { identifier_ = identifier; }
		int getID() { return identifier_; }

		string getName() { return name_; }
		void setName(string name) { name_ = name; }

		Colour2 getColour() { return colour_; }
		void setColour(Colour2 colour) { colour_ = colour; }

		ProvinceMV* getCapital() { return capital_province_; }
		void setCapital(ProvinceMV* province) { if (ownsProvince(province)) capital_province_ = province; }

		vector<ProvinceMV*> getOwnedProvinces() { return owned_provinces_; }
		int getNumberProvinces() { return owned_provinces_.size(); }
		bool ownsProvince(ProvinceMV* province) { return find(owned_provinces_.begin(), owned_provinces_.end(), province) != owned_provinces_.end(); }
		void addProvince(ProvinceMV* province) { owned_provinces_.push_back(province); }
		void removeProvince(ProvinceMV* province) {
			if (ownsProvince(province)) {
				if (getCapital() != province) {
					owned_provinces_.erase(remove(owned_provinces_.begin(), owned_provinces_.end(), province), owned_provinces_.end());
				} else if (getNumberProvinces() > 1) { // Can relocate capital if the nation owns multiple provinces
					owned_provinces_.erase(remove(owned_provinces_.begin(), owned_provinces_.end(), province), owned_provinces_.end());
					setCapital(getOwnedProvinces()[0]);
				}
			}
		}

		vector<Unit*> getOwnedUnits() { return owned_army_units_; }
		void addUnit(Unit* unit) { owned_army_units_.push_back(unit); }

		double getMoney() { return money_; }
		void setMoney(double money) { money_ = money; }
		void addMoney(double amount) { money_ += amount; }

		double getIncome() { return income_; }
		void setIncome(double income) { income_ = income; }
		void addIncome(double amount) { income_ += amount; }

		double getOutgoings() { return outgoings_; }
		void setOutgoings(double outgoings) { outgoings_ = outgoings; }
		void addOutgoing(double amount) { outgoings_ += amount; }

		bool isInDebt() { return in_debt_; }

		void evaluate() {
			double final_income = income_; // Final income is base income plus value from all owned provinces
			for (ProvinceMV* province : getOwnedProvinces()) {
				province->evaluate();
				final_income += province->getValue();
			}
			for (Unit* unit : getOwnedUnits()) {
				unit->evaluate();
			}
			money_ += final_income - outgoings_;
			in_debt_ = money_ < 0.00 ? true : false;
		}

		bool hireUnit(int size, double skill = 1.00) {
			double cost = size * UNIT_COST * skill;
			if (money_ <= cost) return false;
			addMoney(-cost);
			addUnit(new Unit(UnitCount++, this, size, skill, getCapital()));
			return true;
		}

		void dismissUnit(Unit* unit) {
			owned_army_units_.erase(remove(owned_army_units_.begin(), owned_army_units_.end(), unit), owned_army_units_.end());
			delete unit;
		}
};
