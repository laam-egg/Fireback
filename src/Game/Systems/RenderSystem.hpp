#ifndef RenderSystem_INCLUDED
#define RenderSystem_INCLUDED

#include "system_begin_code.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

class RenderSystem
	: public BaseSystem {
public:
	static Signature getSignature();

	void init();

	void quit();

	enum Alignment {
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER,
		ALIGN_TOP,
		ALIGN_BOTTOM
	};

	SDL_Rect drawText(Vector position, std::string const& s, TTF_Font* const font, SDL_Color const& fontColor, Alignment horizontalAlign = ALIGN_LEFT, Alignment verticalAlign = ALIGN_TOP);

	void update();
};

#endif // RenderSystem_INCLUDED
