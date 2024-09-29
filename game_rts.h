#pragma once

#include "game.h"

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
		Date date = { 800, 1, 1 };

		bool value_view = false;
		bool game_paused = false;
		bool picking_nation = false;;
		
		void extendedInitialisation() override;
		void updateObjects(float modifier = 1.0f) override;
		void updateStatistics(int frames, int updates) override;
		void updateProperties() override;
		void pauseGame();
		void setupRTSGame();
		void moveUnit(Province* province);
		void hoverProvince(Province* province);
		void hoverUnit(Unit* unit);
		void updateCursor();
		void executeAction(BUTTON_ACTION action);
		void incrementDay();
		int  gameLoop() override;

		string getDate();
		Moveable* getObjectUnderMouse() override;

		GameRTS() : Game() {}
		GameRTS(int a, char** b) : Game(a, b) {}
};
