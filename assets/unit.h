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

		void initialise();
		void evaluate();
		void inflict_fatalities(int amount);
		void initiate_battle(Unit* unit);
		void receive_battle(Unit* unit, BattleInformation* battle_info);
		void finish_battle();
		void advance_path();
		void set_path(vector<Province*> path);
		vector<Province*> get_path() { return travel_path_; }

		void set_id(int identifier) { identifier_ = identifier; }
		int get_id() { return identifier_; }

		int get_amount() { return amount_; }
		void set_amount(int amount) { amount_ = amount; }

		float get_skill() { return skill_factor_; }
		void set_skill(float skill) { skill_factor_ = skill; }

		float get_speed() { return speed_; }
		void set_speed(float speed) { speed_ = speed; }

		UNIT_STATE get_state() { return state_; }
		void set_state(UNIT_STATE state) { state_ = state; }

		Province* get_province() { return province_; }
		void set_province(Province* province) { province_ = province; }

		Province* get_target_province() { return target_province_; }
		void set_target_province(Province* province) { target_province_ = province; }

		Nation* get_nation() { return nation_; }
		void set_nation(Nation* nation) { nation_ = nation; }
};
