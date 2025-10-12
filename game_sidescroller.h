#pragma once

#include "game.h"

using namespace std;

class GameSidescroller : Game {
	public:
		const float GRAVITY = 0.004f;
		bool traversed = false;
		
		void check_collision();
		void initialise_extended() override;
		void update_objects(float modifier = 1.0f) override;
		int game_loop() override;

		virtual ~GameSidescroller() {}
		GameSidescroller() : Game() {}
		GameSidescroller(int a, char** b) : Game(a, b) {}
};
