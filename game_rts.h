#pragma once

#include "game.h"

enum WarGoal {TAKE_KEY_PROVINCE, TAKE_MULTIPLE_PROVINCES, DEFEAT_ARMY, VASSALISE};

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

struct Date { short year, month, day; };
const short month_days[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const string month_names[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

static void loadProvinceNeighbours(string neighbours_file);
static void loadProvinceAttributes(string attributes_file);

using namespace std;

class GameRTS : Game {
	public:
		vector<Nation*> nations;
		Nation* player_nation = nullptr;
		Nation* viewed_nation = nullptr;
		Date date = { 800, 1, 1 };

		bool value_view = false;
		bool game_paused = false;
		bool simulation_paused = false;
		bool picking_nation = false;
		
		void extendedInitialisation() override;
		void updateObjects(float modifier = 1.0f) override;
		void updateStatistics(int frames, int updates) override;
		void updateProperties() override;
		void pauseGame();
		void setupRTSGame();
		void expandNation(Province* province);
		void moveUnit(Province* province);
		void hoverProvince(Province* province);
		void hoverUnit(Unit* unit);
		void updateCursor();
		void executeAction(BUTTON_ACTION action, Moveable* button = nullptr);
		void incrementDay();
		int  gameLoop() override;
		// void evaluateNations(const std::vector<Nation*>& nations);

		string getDate();
		Moveable* getObjectUnderMouse() override;

		GameRTS() : Game() {}
		GameRTS(int a, char** b) : Game(a, b) {}
};
