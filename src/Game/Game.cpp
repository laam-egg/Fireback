#include "Game/Game.hpp"
#include "Game/Systems/AllSystems.hpp"

#include "Core/ECS/ecs_registry.hpp"
USING_COMPONENT(Renderable);
USING_COMPONENT(Transform);
USING_COMPONENT(RigidBody);
USING_COMPONENT(Collider);
USING_COMPONENT(Bullet);
USING_COMPONENT(Player);
USING_COMPONENT(Bot);

USING_SYSTEM(RenderSystem);
USING_SYSTEM(KinematicsSystem);
USING_SYSTEM(DynamicsSystem);
USING_SYSTEM(CollisionSystem);
USING_SYSTEM(PlayerSystem);
USING_SYSTEM(BotSystem);

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <chrono>
#include <sstream>
#include <iomanip>

#include "Core/Timer.hpp"

#ifdef G
#undef G
#endif

#define G (*this)

std::string Game::formatAge(Scalar age) {
	std::chrono::milliseconds ms(static_cast<std::uint64_t>(age));
	auto secs = std::chrono::duration_cast<std::chrono::seconds>(ms);
	ms -= std::chrono::duration_cast<std::chrono::milliseconds>(secs);
	auto mins = std::chrono::duration_cast<std::chrono::minutes>(secs);
	secs -= std::chrono::duration_cast<std::chrono::seconds>(mins);
	auto hours = std::chrono::duration_cast<std::chrono::hours>(mins);
	mins -= std::chrono::duration_cast<std::chrono::minutes>(hours);

	std::stringstream ss;
	ss << std::setw(2) << std::setfill('0') << hours.count() << ":";
	ss << std::setw(2) << std::setfill('0') << mins.count() << ":";
	ss << std::setw(2) << std::setfill('0') << secs.count() << ".";
	ss << std::setw(3) << std::setfill('0') << ms.count();
	return ss.str();
}

std::string Game::getFullResourcePath(std::string const& path) const {
	return m_basePath + "/res/" + path;
}

SDL_Texture* Game::loadImageResourceAsTexture(std::string path) const {
	path = G.getFullResourcePath(path);
	SDL_Surface* surface = IMG_Load(path.c_str());
	if (surface == NULL) {
		throw Exception(std::string("Could not load image. IMG_GetError(): ") + IMG_GetError());
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		throw Exception(std::string("Could not create texture. SDL_GetError(): ") + SDL_GetError());
	}
	SDL_FreeSurface(surface);
	return texture;
}

TTF_Font* Game::openFont(std::string path, int pointSize) const {
	path = G.getFullResourcePath(path);
	TTF_Font* font = TTF_OpenFont(path.c_str(), pointSize);
	if (font == NULL) {
		throw Exception(std::string("Could not open font. TTF_GetError(): ") + TTF_GetError());
	}
	return font;
}

void Game::createBullet(Vector const& position, Scalar gunAngle, BulletEmitter bulletEmitter) {
	Vector velocity(0.2f, 0);
	velocity.rotate(gunAngle);

	EntityID entityID = G.ecs.createEntity();
	G.ecs.addComponentToEntity(entityID, Transform(position, Scalar(0), Scalar(3)));
	G.ecs.addComponentToEntity(entityID, RigidBody(Scalar(20), velocity, Vector(0, 0)));
	G.ecs.addComponentToEntity(entityID, Collider());
	G.ecs.addComponentToEntity(entityID, Renderable(bulletEmitter == EMITTER_PLAYER ? G.blueBulletTexture : G.redBulletTexture));
	G.ecs.addComponentToEntity(entityID, Bullet(bulletEmitter));
}

void Game::createFire(Vector const& position) {
	EntityID entityID = G.ecs.createEntity();
	G.ecs.addComponentToEntity(entityID, Renderable(G.fireTexture));
	G.ecs.addComponentToEntity(entityID, Transform(position, 0, 28));

	// ATTENTION: Placement-new below.
	static unsigned char timerMemoryPlace[sizeof(Timer)];
	static bool timerInitializedBefore = false;

	if (timerInitializedBefore) {
		reinterpret_cast<Timer*>(timerMemoryPlace)->~Timer();
	}
	else {
		timerInitializedBefore = true;
	}

	auto* t = new(timerMemoryPlace) Timer(10, [entityID](Uint32 interval, TimerCallbackData*) -> Uint32 {
		Transform& tf = Game::instance()/*instead of G*/.ecs.getComponentDataOfEntityAsRef<Transform>(entityID);
		tf.radius += 1;
		if (tf.radius >= 50) return 0;
		else return interval;
	}, NULL, false);

	t->run();
	// No memory leak here thanks to placement-new.
}

void Game::run() {
	SDL_Event event;
	EntityID playerID, botID;

	for (;;) {
		G.gamePlaying = true;
		G.age = 0;
		G.lastAgeMerit = 0;
		G.bulletsLeft = 500;
		G.level = 0;

		G.dt = 0;

		G.ecs.restart();

		playerID = G.ecs.createEntity();
		{
			G.ecs.addComponentToEntity(playerID, Player());
			G.ecs.addComponentToEntity(playerID, Transform(Vector(20, 20), 0, 10));
			G.ecs.addComponentToEntity(playerID, Collider());
			G.ecs.addComponentToEntity(playerID, Renderable(G.playerTexture));
		}

		botID = G.ecs.createEntity();
		{
			G.ecs.addComponentToEntity(botID, Bot());
			G.ecs.addComponentToEntity(botID, Transform(Vector(G.windowWidth / 2, G.windowHeight / 2), 0, 15));
			G.ecs.addComponentToEntity(botID, Renderable(G.botTexture));
		}

		while (G.gamePlaying) {
			auto startTime = std::chrono::high_resolution_clock::now();

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_QUIT:
					G.gamePlaying = false;
					return;
				}
			}

			G.rend->update();

			G.kin->update(G.dt);
			G.col->update(G.dt);
			G.dyn->update(G.dt);

			G.pls->update(G.dt);
			G.bos->update(G.dt);

			if (G.age - G.lastAgeMerit >= 20000 * G.level) {
				G.bulletsLeft += G.level * 3000;
				++G.level;
				G.lastAgeMerit = G.age;
			}

			auto stopTime = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration<Scalar, std::chrono::milliseconds::period>(stopTime - startTime);
			G.dt = duration.count();
			G.age += G.dt;
		}

		bool waitForUserResponse = true;
		while (waitForUserResponse) {
			G.rend->update();
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_QUIT:
					waitForUserResponse = false;
					return;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_RETURN:
						waitForUserResponse = false;
						break;
					}
				}
			}
			SDL_Delay(10);
		}
	}
}

void Game::init() {
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			throw Exception(SDL_GetError());
		}

		int img = IMG_INIT_PNG; // SDL_image from vcpkg does not support JPEG (2023 Apr 08)
		if ((IMG_Init(img) & img) != img) {
			throw Exception(IMG_GetError());
		}

		if (TTF_Init() != 0) {
			throw Exception(TTF_GetError());
		}

		int mix = MIX_INIT_MP3 | MIX_INIT_MID;
		if ((Mix_Init(mix) & mix) != mix) {
			throw Exception(Mix_GetError());
		}
	}

	m_basePath = SDL_GetBasePath();

	G.window = SDL_CreateWindow("Fireback", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, G.windowWidth, G.windowHeight, SDL_WINDOW_SHOWN);
	if (G.window == NULL) {
		throw Exception(SDL_GetError());
	}
	G.renderer = SDL_CreateRenderer(G.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (G.renderer == NULL) {
		throw Exception(SDL_GetError());
	}

	G.playerTexture = loadImageResourceAsTexture("images/player.png");
	G.botTexture = loadImageResourceAsTexture("images/bot.png");
	G.blueBulletTexture = loadImageResourceAsTexture("images/blue-bullet.png");
	G.redBulletTexture = loadImageResourceAsTexture("images/red-bullet.png");
	G.fireTexture = loadImageResourceAsTexture("images/fire.png");

	G.ecs.init();

	G.ecs.registerComponent<Transform>();
	G.ecs.registerComponent<RigidBody>();
	G.ecs.registerComponent<Collider>();
	G.ecs.registerComponent<Renderable>();
	G.ecs.registerComponent<Player>();
	G.ecs.registerComponent<Bot>();
	G.ecs.registerComponent<Bullet>();

	G.rend = G.ecs.registerSystem<RenderSystem>(RenderSystem::getSignature());
	G.rend->init(G.renderer);

	G.col = G.ecs.registerSystem<CollisionSystem>(CollisionSystem::getSignature());
	G.col->init();

	G.kin = G.ecs.registerSystem<KinematicsSystem>(KinematicsSystem::getSignature());
	G.kin->init();

	G.dyn = G.ecs.registerSystem<DynamicsSystem>(DynamicsSystem::getSignature());
	G.dyn->init();

	G.pls = G.ecs.registerSystem<PlayerSystem>(PlayerSystem::getSignature());
	G.pls->init();

	G.bos = G.ecs.registerSystem<BotSystem>(BotSystem::getSignature());
	G.bos->init();
}

void Game::quit() {
	G.kin->quit();
	G.col->quit();
	G.dyn->quit();
	G.pls->quit();
	G.bos->quit();
	G.rend->quit();

	SDL_DestroyTexture(G.fireTexture);
	SDL_DestroyTexture(G.redBulletTexture);
	SDL_DestroyTexture(G.blueBulletTexture);
	SDL_DestroyTexture(G.botTexture);
	SDL_DestroyTexture(G.playerTexture);

	SDL_DestroyRenderer(G.renderer);

	SDL_DestroyWindow(G.window);

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
