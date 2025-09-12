#pragma once

#include "game.h"

#include <string>
#include <vector>

using namespace std;

class Moveable;
class Nation;
class Province;
class Unit;

enum WarGoal {TAKE_KEY_PROVINCE, TAKE_MULTIPLE_PROVINCES, DEFEAT_ARMY, VASSALISE};
enum Event {BATTLE_START, BATTLE_END};

static int BattleID = 0;

struct War {
	Nation* attacker;
	Nation* defender;
	vector<Nation*> attacker_allies;
	vector<Nation*> defender_allies;
	WarGoal war_goal = TAKE_KEY_PROVINCE;

	union WarGoalTarget {
		Province* target_province;
		int target_number;
	} war_goal_target;

	vector<Province*>* provinces_sieged_by_attacker;
	vector<Province*>* provinces_sieged_by_defender;
	int get_province_swing() { return provinces_sieged_by_attacker->size() - provinces_sieged_by_defender->size(); }
	int army_swing = 0;
};

struct HeaderInformation {
	const float& money;
	const string& date;
};

struct BattleInformation {
	vector<Unit*> attacker_units; // 1st element always the main attacker
	vector<Unit*> defender_units; // 1st element always the main defender
	int total_attacker_starting_strength;
	int total_defender_starting_strength;
	int total_attacker_current_strength = 0;
	int total_defender_current_strength = 0;
	int defender_losses;
	Province* province;
	int get_battle_swing() { return total_attacker_current_strength - total_defender_current_strength; }
	int battle_id = BattleID++;
};

const short MONTH_DAYS[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const string MONTH_NAMES[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

struct Date {
	short year, month, day; 

	void increment() {
		if (++day > MONTH_DAYS[month]) {
			day = 1;
			if (++month > 12) {
				month = 1;
				++year;
			}
		}
	}

	string format() {
		string day_suffix = "th";
		if (day % 100 < 11 || day % 100 > 13) {
			switch (day % 10) {
				case 1: day_suffix = "st"; break;
				case 2: day_suffix = "nd"; break;
				case 3: day_suffix = "rd"; break;
			}
		}
		return to_string(day) + day_suffix + " " + MONTH_NAMES[month - 1] + " " + to_string(year) + " AD";
	}
};

static void load_province_neighbours(string neighbours_file);
static void load_province_attributes(string attributes_file);

using namespace std;

class GameRTS : Game {
	private:
		Moveable* cursor;
	
	public:
		static GameRTS* instance;
		vector<Nation*> nations;
		vector<BattleInformation*> battles;
		Nation* player_nation = nullptr;
		Nation* viewed_nation = nullptr;
		Date date = { 800, 1, 1 };

		bool value_view = false;
		bool game_paused = false;
		bool simulation_paused = false;
		bool picking_nation = false;

		int game_loop() override;
		void initialise_extended() override;
		void update_objects(const float& modifier = 1.0f) override;
		void update_statistics(const int& frames, const int& updates) override;
		void update_properties() override;
		void update_cursor() override;
		Moveable* get_object_under_mouse() override;

		void pause_game();
		void initialise_rts_game();
		void expand_nation(Province* province);
		void move_unit(Province* province);
		void hover_province(Province* province);
		void hover_unit(Unit* unit);
		void execute_action(int action, Moveable* button = nullptr);
		void register_event(Event event, void* details = nullptr);

		GameRTS();
		GameRTS(int a, char** b);
};
