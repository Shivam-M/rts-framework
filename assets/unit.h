#pragma once

#include "moveable.h"

#include <vector>
#include <string>
#include <cstdint>

using namespace std;

class Nation;
class Province;
class Text;
struct BattleInformation;

static int PathCount = 0;

class Unit: public Moveable {
	private:
		vector<Province*> path_;
		Province* target_ = nullptr;
		Unit* enemy_ = nullptr;
		Blend siege_lines = Blend(2, 1.0f, 1.0f, Vector2(-1.f, -1.5f), false);
		BattleInformation* battle_ = nullptr;

	public:
		enum class State : uint8_t { Normal, Sieging, Travelling, Fighting, Dead };

		int identifier = -1;
		int amount = 1;
		float speed = 0.01f;
		float skill = 1.00f;
		Province* province = nullptr;
		Nation* nation = nullptr;
		State state = State::Normal;

		virtual ~Unit() {};
		Unit(int id, Nation* owner_nation, int size, float skill_factor = 1.00f, Province* starting_province = nullptr);

		void initialise();
		void evaluate();
		void inflict_fatalities(int amount);
		void initiate_battle(Unit* unit);
		void receive_battle(Unit* unit, BattleInformation* battle_info);
		void finish_battle();
		void advance_path();
		void set_path(vector<Province*> path);
};
