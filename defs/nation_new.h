#pragma once

static float UNIT_COST = 0.02;
static int UnitCount = 0;

enum NATION_FLAGS {
	AT_WAR,
	LOSING_MONEY,
	IN_DEBT,
	UNLANDED,
};

class NationNew : public Moveable {
	private:
		int identifier_ = -1;
		string name_ = "Generic Nation";
		Colour colour_ = Colour(255, 255, 255);
		Province* capital_province_;
		vector<Province*> owned_provinces_;
		vector<Unit*> owned_army_units_;
		float money_ = 1.00;
		float income_ = 0.00;
		float outgoings_ = 0.00;
		bool in_debt_ = false;
		int flags = 0;

	public:
		NationNew(int identifier, string name, Province* capital = nullptr) : identifier_(identifier), name_(name), capital_province_(capital) {}

		void setID(int identifier) { identifier_ = identifier; }
		int getID() { return identifier_; }

		Province* getCapital() { return capital_province_; }
		void setCapital(Province* province) { if (ownsProvince(province)) capital_province_ = province; }

		vector<Province*> getOwnedProvinces() { return owned_provinces_; }
		int getNumberProvinces() { return owned_provinces_.size(); }
		bool ownsProvince(Province* province) { return find(owned_provinces_.begin(), owned_provinces_.end(), province) != owned_provinces_.end(); }
		void addProvince(Province* province) { owned_provinces_.push_back(province); }
		void removeProvince(Province* province) {
			if (ownsProvince(province)) {
				if (getCapital() != province) {
					owned_provinces_.erase(remove(owned_provinces_.begin(), owned_provinces_.end(), province), owned_provinces_.end());
				}
				else if (getNumberProvinces() > 1) { // Can relocate capital if the nation owns multiple provinces
					owned_provinces_.erase(remove(owned_provinces_.begin(), owned_provinces_.end(), province), owned_provinces_.end());
					setCapital(getOwnedProvinces()[0]);
				}
			}
		}

		vector<Unit*> getOwnedUnits() { return owned_army_units_; }
		void addUnit(Unit* unit) { owned_army_units_.push_back(unit); }

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

		void evaluate() {
			float final_income = income_; // Final income is base income plus value from all owned provinces
			for (Province* province : getOwnedProvinces()) {
				province->evaluate();
				final_income += province->getValue();
			}
			for (Unit* unit : getOwnedUnits()) {
				unit->evaluate();
			}
			money_ += final_income - outgoings_;
			in_debt_ = money_ < 0.00 ? true : false;
		}

		bool hireUnit(int size, float skill = 1.00) {
			float cost = size * UNIT_COST * skill;
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
