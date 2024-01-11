#define GAMEV2

#ifdef GAMEV2

#include "game_new.h"

int main(int argc, char** argv) {
	GameNew game = GameNew(argc, argv);
	return game.gameLoop();
}

#else

#include "game.h"

int main(int argc, char** argv) {
	Game game = Game(argc, argv);
	return game.gameLoop();
}

#endif