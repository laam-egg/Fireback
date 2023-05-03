#include "Game/Systems/RenderSystem.hpp"
#include "Game/Game.hpp"

Signature RenderSystem::getSignature() {
	Signature sig;
	sig.enableComponent(G.ecs.getComponentID<Transform>());
	sig.enableComponent(G.ecs.getComponentID<Renderable>());
	return sig;
}

RenderSystem::RenderSystem()
	: m_renderer(NULL), m_digiFont(NULL) {

}

void RenderSystem::init(SDL_Renderer* renderer) {
	m_renderer = renderer;
	m_digiFont = G.openFont("fonts/DS-DIGI.ttf", G.statusAreaRect.h - 1);
}

void RenderSystem::quit() {
	TTF_CloseFont(m_digiFont);
}

SDL_Rect RenderSystem::drawText(Vector position, std::string const& s, TTF_Font* const font, SDL_Color const& fontColor, Alignment horizontalAlign, Alignment verticalAlign) {
	SDL_Surface* fontSf = TTF_RenderText_Solid(font, s.c_str(), fontColor);
	SDL_Texture* fontTex = SDL_CreateTextureFromSurface(m_renderer, fontSf);
	Vector size(fontSf->w, fontSf->h);
	switch (horizontalAlign) {
	case ALIGN_RIGHT:
		position.x -= size.x;
		break;

	case ALIGN_CENTER:
		position.x -= size.x / 2;
		break;

	default:
		break;
	}

	switch (verticalAlign) {
	case ALIGN_BOTTOM:
		position.y -= size.y;
		break;

	case ALIGN_CENTER:
		position.y -= size.y / 2;
		break;

	default:
		break;
	}
	SDL_Rect fontDstRect = { static_cast<int>(position.x), static_cast<int>(position.y), fontSf->w, fontSf->h };
	SDL_RenderCopy(m_renderer, fontTex, NULL, &fontDstRect);
	SDL_DestroyTexture(fontTex);
	//SDL_FreeSurface(fontSf);
	// The above line causes heap corruption error (return code 0xC0000374) when the program is
	// compiled with Code::Blocks 20.03 (MinGW 8.1.0). However, it is recommended that the surface
	// returned by TTF_RenderText_Solid() be freed when no longer used (*). In fact, when this line
	// is present, VS 2022 compiles and runs the program just fine.
	// It should therefore be considered a compiler's fault (or SDL2's fault on that compiler).
	// I don't know whether other versions of MinGW-GCC have the same issue, so just ignore that
	// line in case the (known so far) problematic compiler version is detected.
	// (*) See these tutorials:
	// 1. https://lazyfoo.net/tutorials/SDL/16_true_type_fonts/index.php#:~:text=SDL_FreeSurface(%20textSurface%20)%3B
	// 2. https://thenumb.at/cpp-course/sdl2/07/07.html#:~:text=SDL_FreeSurface(%20text%20)%3B
	// Even though I tried swapping texture-destroying and surface-freeing code, the problem persists.

#ifdef __GNUC__ // https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#define GCC_VERSION 0
#endif // __GNUC__

#if GCC_VERSION == 80100 // MinGW-GCC 8.1.0
    // SDL_FreeSurface(fontSf); // accepting possible memory leak
#else
    SDL_FreeSurface(fontSf);
#endif // GCC_VERSION
	return fontDstRect;
}

void RenderSystem::update() {
	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
	SDL_RenderClear(m_renderer);

	SDL_RenderSetViewport(m_renderer, &G.statusAreaRect);
	std::string sTime = G.formatAge(G.age);
	SDL_Color fontColor = { 0, 255, 0, 255 };
	if (!G.gamePlaying) {
		fontColor.r = 255;
		fontColor.g = 0;
		fontColor.b = 0;
		fontColor.a = 255;
	}
	{
		drawText(Vector(1, 1), sTime, m_digiFont, fontColor);

		if (G.gamePlaying) {
			fontColor.r = 0;
			fontColor.g = 0;
			fontColor.b = 255;
			int a = 128 + G.level * 10;
			if (a >= 256) a = 255;
			fontColor.a = static_cast<Uint8>(a);
		}
		drawText(Vector(G.statusAreaRect.w - 3, 1), std::to_string(G.bulletsLeft), m_digiFont, fontColor, ALIGN_RIGHT);
	}

	SDL_RenderSetViewport(m_renderer, &G.playgroundRect);
	for (EntityID const& entityID : entityIDs) {
		Transform& tf = G.ecs.getComponentDataOfEntityAsRef<Transform>(entityID);
		Renderable& rd = G.ecs.getComponentDataOfEntityAsRef<Renderable>(entityID);

		if (rd.texture == NULL) {
			throw Exception(std::string("Texture not yet set for this Renderable entity. Entity ID: ") + std::to_string(entityID));
		}

		SDL_Rect dst{};
		dst.x = static_cast<int>(tf.position.x - tf.radius);
		dst.y = static_cast<int>(tf.position.y - tf.radius);
		dst.w = dst.h = static_cast<int>(tf.radius * 2);
		SDL_RenderCopyEx(m_renderer, rd.texture, NULL, &dst, tf.rotation * 180 / 3.14, NULL, SDL_FLIP_NONE);
	}

	SDL_RenderSetViewport(m_renderer, NULL);

	if (!G.gamePlaying) {
		SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 127);
		SDL_RenderFillRect(m_renderer, NULL);

		fontColor.r = 255;
		fontColor.g = 255;
		fontColor.b = 255;
		fontColor.a = 255;
#define drawTextGameOver() drawText(Vector(G.windowWidth, G.windowHeight) / 2, "GAME OVER", m_digiFont, fontColor, ALIGN_CENTER, ALIGN_CENTER)
		SDL_Rect r = drawTextGameOver();
		SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(m_renderer, &r);
		drawTextGameOver();
	}

	SDL_RenderPresent(m_renderer);
}
