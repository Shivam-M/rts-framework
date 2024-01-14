#include "game.h"

int main(int argc, char** argv) {
	Game game = Game(argc, argv);
	return game.gameLoop();
}
