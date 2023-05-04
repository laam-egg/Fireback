#ifndef Game_INCLUDED
#define Game_INCLUDED

#include "Core/Vector.hpp"
#include "Core/ECS/ECS.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

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

	///////////////////////
	// UI Justifications //
	///////////////////////
	int const windowWidth{ 640 };
	int const windowHeight{ 480 };
	int const statusAreaHeight{ 40 };

	SDL_Rect const playgroundRect{ 0, statusAreaHeight, windowWidth, windowHeight - statusAreaHeight };
	SDL_Rect const statusAreaRect{ 0, 0, windowWidth, statusAreaHeight };

	////////////////////
	// Game variables //
	////////////////////
	bool gamePlaying{ false };

	Scalar age{ 0 };
	Scalar lastAgeMerit{ 0 };
	int bulletsLeft{ 500 };
	int level{ 0 };

	Scalar dt{ 0 };

	//////////////
	// Back-end //
	//////////////
	SDL_Window* window{ NULL };
	SDL_Renderer* renderer{ NULL };

#define DECLARE_SYSTEM_PTR_VAR(sys, varName) std::shared_ptr<sys> varName{ nullptr }
	DECLARE_SYSTEM_PTR_VAR(RenderSystem, rend);
	DECLARE_SYSTEM_PTR_VAR(CollisionSystem, col);
	DECLARE_SYSTEM_PTR_VAR(KinematicsSystem, kin);
	DECLARE_SYSTEM_PTR_VAR(DynamicsSystem, dyn);
	DECLARE_SYSTEM_PTR_VAR(PlayerSystem, pls);
	DECLARE_SYSTEM_PTR_VAR(BotSystem, bos);

	///////////////
	// Resources //
	///////////////
	SDL_Texture* playerTexture{ NULL };
	SDL_Texture* botTexture{ NULL };
	SDL_Texture* blueBulletTexture{ NULL };
	SDL_Texture* redBulletTexture{ NULL };
	SDL_Texture* fireTexture{ NULL };

	TTF_Font* digiFont{ NULL };

	Mix_Chunk* botGunShotSound{ NULL };
	Mix_Chunk* playerGunShotSound{ NULL };
	Mix_Chunk* fireSound{ NULL };

	static constexpr int const botGunShotSoundChannel{ 0 };
	static constexpr int const playerGunShotSoundChannel{ 1 };
	static constexpr int const fireSoundChannel{ 2 };
	static constexpr int const numSoundChannels{ 1 + 3 };
	// Add one more here ........................^
	// perhaps because we have to take the background music channel
	// into account, too.

	///////////////////////
	// Utility functions //
	///////////////////////
	static std::string formatAge(Scalar age);

	std::string getFullResourcePath(std::string const& path) const;

	/** ATTENTION: Destroy the returned texture using SDL_DestroyTexture() at exit. */
	SDL_Texture* loadImageResourceAsTexture(std::string const& path) const;

	/** ATTENTION: Close the returned font using TTF_CloseFont() at exit. */
	TTF_Font* openFont(std::string const& path, int pointSize) const;

	/** ATTENTION: Free the returned audio chunk using Mix_FreeChunk() at exit. 
	 * ALSO NOTE THAT this function is able to load WAV files only. */
	Mix_Chunk* loadAudioChunk(std::string const& path) const;

	void createBullet(Vector const& position, Scalar gunAngle, BulletEmitter bulletEmitter);

	void createFire(Vector const& position);

	////////////////////
	// Main functions //
	////////////////////
	void run();

	void init();

	void quit();

private:
	std::string m_basePath;
};

#define G Game::instance()

#endif // Game_INCLUDED
