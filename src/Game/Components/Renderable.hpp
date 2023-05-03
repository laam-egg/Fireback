#ifndef Renderable_COMPONENT_INCLUDED
#define Renderable_COMPONENT_INCLUDED

#include <SDL2/SDL.h>

struct Renderable {
	SDL_Texture* texture{ NULL };

	Renderable() = default;

	Renderable(SDL_Texture* tex)
		: texture(tex) {

	}
};

#endif // Renderable_COMPONENT_INCLUDED
