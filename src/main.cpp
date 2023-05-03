// VELOCITY UNIT: pixel per millisecond.

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "Game/Game.hpp"

extern "C" int main(int argc, char* argv[]) {
	G.init();
	G.run();
	G.quit();
	return 0;
}
