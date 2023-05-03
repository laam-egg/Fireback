#ifndef Game_INCLUDED
#define Game_INCLUDED

#include "Core/Vector.hpp"
#include "Core/ECS/ECS.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Game/BulletEmitter.hpp"

// Forward declarations
class RenderSystem;
class CollisionSystem;
class KinematicsSystem;
class DynamicsSystem;
class PlayerSystem;
class BotSystem;

class Game {
public:
	// Singleton
	inline static Game& instance() {
		static Game g;
		return g;
	}

	ECS ecs{};
	int const windowWidth{ 640 };
	int const windowHeight{ 480 };
	int const statusAreaHeight{ 40 };

	SDL_Rect const playgroundRect{ 0, statusAreaHeight, windowWidth, windowHeight - statusAreaHeight };
	SDL_Rect const statusAreaRect{ 0, 0, windowWidth, statusAreaHeight };

	bool gamePlaying{ false };

	Scalar age{ 0 };
	Scalar lastAgeMerit{ 0 };
	int bulletsLeft{ 500 };
	int level{ 0 };

	Scalar dt{ 0 };

	SDL_Window* window{ NULL };
	SDL_Renderer* renderer{ NULL };

#define DECLARE_SYSTEM_PTR_VAR(sys, varName) std::shared_ptr<sys> varName{ nullptr }
	DECLARE_SYSTEM_PTR_VAR(RenderSystem, rend);
	DECLARE_SYSTEM_PTR_VAR(CollisionSystem, col);
	DECLARE_SYSTEM_PTR_VAR(KinematicsSystem, kin);
	DECLARE_SYSTEM_PTR_VAR(DynamicsSystem, dyn);
	DECLARE_SYSTEM_PTR_VAR(PlayerSystem, pls);
	DECLARE_SYSTEM_PTR_VAR(BotSystem, bos);

	SDL_Texture* playerTexture{ NULL };
	SDL_Texture* botTexture{ NULL };
	SDL_Texture* blueBulletTexture{ NULL };
	SDL_Texture* redBulletTexture{ NULL };
	SDL_Texture* fireTexture{ NULL };

	static std::string formatAge(Scalar age);

	std::string getFullResourcePath(std::string const& path) const;

	/**
	 * ATTENTION: Destroy the returned texture using SDL_DestroyTexture() at exit.
	 */
	SDL_Texture* loadImageResourceAsTexture(std::string path) const;

	/**
	 * ATTENTION: Disconnect the returned font using TTF_CloseFont() at exit.
	 */
	TTF_Font* openFont(std::string path, int pointSize) const;

	void createBullet(Vector const& position, Scalar gunAngle, BulletEmitter bulletEmitter);

	void createFire(Vector const& position);

	void run();

	void init();

	void quit();

private:
	std::string m_basePath;
};

#define G Game::instance()

#endif // Game_INCLUDED
