// #include "game.h"
#define SSw
#ifdef SS
#include "game_sidescroller.h"
#else
#include "game_rts.h"
#endif
int main(int argc, char** argv) {
	// Game game = Game(argc, argv);
#ifdef SS
	GameSidescroller game = GameSidescroller(argc, argv);
#else
	GameRTS game = GameRTS(argc, argv);
#endif
	game.extendedInitialisation();
	return game.gameLoop();
}
