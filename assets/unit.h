#pragma once

#include "moveable.h"

#include <vector>
#include <string>

using namespace std;

class Nation;
class Province;
class Text;
struct BattleInformation;

static int PathCount = 0;

class Unit: public Moveable {
	private:
		int identifier_ = -1;
		int amount_ = 1;
		float skill_factor_ = 1.00f;
		float speed_ = 0.01f;
		Nation* nation_ = nullptr;
		Province* province_ = nullptr;
		Province* target_province_ = nullptr;
		vector<Province*> travel_path_;
		Unit* enemy_unit_ = nullptr;
		Blend blend_sieged = Blend(2, 1.0f, 1.0f, Vector2(-1.f, -1.5f), false);
		BattleInformation* battle_info_ = nullptr;

	public:
		enum UNIT_STATE { NORMAL, SIEGING, TRAVELLING, FIGHTING, DEAD };
		UNIT_STATE state_ = NORMAL;

		Unit(int identifier, Nation* nation, int size, float skill = 1.00f, Province* province = nullptr);

		void setID(int identifier) { identifier_ = identifier; }
		int getID() { return identifier_; }

		int getAmount() { return amount_; }
		void setAmount(int amount) { amount_ = amount; }

		float getSkill() { return skill_factor_; }
		void setSkill(float skill) { skill_factor_ = skill; }

		float getSpeed() { return speed_; }
		void setSpeed(float speed) { speed_ = speed; }

		UNIT_STATE getState() { return state_; }
		void setState(UNIT_STATE state) { state_ = state; }

		Province* getProvince() { return province_; }
		void setProvince(Province* province) { province_ = province; }

		Province* getTarget() { return target_province_; }
		void setTarget(Province* province) { target_province_ = province; }

		vector<Province*> getPath() { return travel_path_; }

		void setPath(vector<Province*> path);

		void advancePath();

		void takeFatalities(int amount);

		void receiveBattle(Unit* unit, BattleInformation* battle_info);

		void endBattle();

		void initiateBattle(Unit* unit);

		void initiate();

		Nation* getNation() { return nation_; }
		void setNation(Nation* nation) { nation_ = nation; }

		void evaluate();
};
