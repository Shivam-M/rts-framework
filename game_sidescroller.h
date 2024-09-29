#pragma once

#include "game.h"

using namespace std;

class GameSidescroller : Game {
	public:
		const float GRAVITY = 0.004f;
		bool traversed = false;
		
		void checkCollision();
		void extendedInitialisation() override;
		void updateObjects(float modifier = 1.0f) override;
		int gameLoop() override;

		GameSidescroller() : Game() {}
		GameSidescroller(int a, char** b) : Game(a, b) {}
};
