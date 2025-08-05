#pragma once

#include "game.h"

enum WarGoal {TAKE_KEY_PROVINCE, TAKE_MULTIPLE_PROVINCES, DEFEAT_ARMY, VASSALISE};
enum Event {START_BATTLE};

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

static void loadProvinceNeighbours(string neighbours_file);
static void loadProvinceAttributes(string attributes_file);

using namespace std;

class GameRTS : Game {
	private:
		Moveable* cursor;
	
	public:
		static GameRTS* instance;
		vector<Nation*> nations;
		Nation* player_nation = nullptr;
		Nation* viewed_nation = nullptr;
		Date date = { 800, 1, 1 };

		bool value_view = false;
		bool game_paused = false;
		bool simulation_paused = false;
		bool picking_nation = false;
		
		void extendedInitialisation() override;
		void updateObjects(const float& modifier = 1.0f) override;
		void updateStatistics(const int& frames, const int& updates) override;
		void updateProperties() override;
		void pauseGame();
		void setupRTSGame();
		void expandNation(Province* province);
		void moveUnit(Province* province);
		void hoverProvince(Province* province);
		void hoverUnit(Unit* unit);
		void updateCursor();
		void executeAction(BUTTON_ACTION action, Moveable* button = nullptr);
		void registerEvent(Event event, void* details = nullptr);
		int  gameLoop() override;

		Moveable* getObjectUnderMouse() override;

		GameRTS();
		GameRTS(int a, char** b);
};
