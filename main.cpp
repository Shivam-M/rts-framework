// #include "game.h"
#define SSs
#ifdef SS
#include "game_sidescroller.h"
#else
#include "game_rts.h"
#endif

// #define DEBUG_MEMORY_LEAKS
#if defined (_WIN32) && defined(_DEBUG) && defined(DEBUG_MEMORY_LEAKS)
#include <crtdbg.h>
#endif

int main(int argc, char** argv) {
#ifdef DEBUG_MEMORY_LEAKS
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// _CrtSetBreakAlloc(-1);
#endif
	// Game game = Game(argc, argv);
#ifdef SS
	GameSidescroller game = GameSidescroller(argc, argv);
#else
	GameRTS game = GameRTS(argc, argv);
#endif
	game.initialise_extended();
	return game.game_loop();
}
