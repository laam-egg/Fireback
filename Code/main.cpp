#include <iostream>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "Core/Vector.hpp"
#include "Core/ECS/ECS.hpp"


// BEGIN COPIED CODE from https://gist.github.com/Gumichan01/332c26f6197a432db91cc4327fcabb1c
// THIS COPIED CODE WILL BE REMOVED IN THE FUTURE
// WHEN ENTITIES ARE RENDERED AS IMAGES.
////////////////////////////////////////////////////////////////////


int
SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
	int offsetx, offsety, d;
	int status;

	offsetx = 0;
	offsety = radius;
	d = radius - 1;
	status = 0;

	while (offsety >= offsetx) {
		status += SDL_RenderDrawPoint(renderer, x + offsetx, y + offsety);
		status += SDL_RenderDrawPoint(renderer, x + offsety, y + offsetx);
		status += SDL_RenderDrawPoint(renderer, x - offsetx, y + offsety);
		status += SDL_RenderDrawPoint(renderer, x - offsety, y + offsetx);
		status += SDL_RenderDrawPoint(renderer, x + offsetx, y - offsety);
		status += SDL_RenderDrawPoint(renderer, x + offsety, y - offsetx);
		status += SDL_RenderDrawPoint(renderer, x - offsetx, y - offsety);
		status += SDL_RenderDrawPoint(renderer, x - offsety, y - offsetx);

		if (status < 0) {
			status = -1;
			break;
		}

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}

	return status;
}


int
SDL_RenderFillCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
	int offsetx, offsety, d;
	int status;

	offsetx = 0;
	offsety = radius;
	d = radius - 1;
	status = 0;

	while (offsety >= offsetx) {

		status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
			x + offsety, y + offsetx);
		status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
			x + offsetx, y + offsety);
		status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
			x + offsetx, y - offsety);
		status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
			x + offsety, y - offsetx);

		if (status < 0) {
			status = -1;
			break;
		}

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}

	return status;
}
////////////////////////////////////////////////////////////////////
// END OF COPIED CODE.


// VELOCITY UNIT: pixel per millisecond.


ECS gEcs;
int const gWindowWidth = 640;
int const gWindowHeight = 480;
SDL_Rect gPlaygroundRect = { 0, 40, 640, 440 };
SDL_Rect gStatusAreaRect = { 0, 0, 640, 40 };
bool gGameRunning = false;
Scalar gAge = 0;
Scalar gLastAgeMerit = 0;
int gBulletsLeft = 500;
int gLevel = 0;

////////////////////////
////// COMPONENTS //////
////////////////////////

struct Renderable {
};

struct Transform {
	Vector position{};
	Scalar rotation{};
	Scalar radius{};

	Transform() = default;

	Transform(Vector position, Scalar rotation, Scalar radius) {
		this->position = position;
		this->rotation = rotation;
		this->radius = radius;
	}
};

struct RigidBody {
	Scalar mass{};
	Vector velocity{};
	Vector acceleration{};

	RigidBody() = default;

	RigidBody(Scalar mass, Vector velocity, Vector acceleration) {
		this->mass = mass;
		this->velocity = velocity;
		this->acceleration = acceleration;
	}
};

struct Collider {
	std::vector<EntityID> collidees{};

	Collider() = default;

	inline bool isCollided() const {
		return !collidees.empty();
	}
};
/*
struct Reflector {
	Vector factors{};

	Reflector() {}

	Reflector(Vector const& factors) {
		this->factors = factors;
	}
};
*/
struct Player {
};

// TODO: Move this class to BotSystem.
// TODO: Documentation for this class.
// It helps bot(s) determine where to shoot.
class BulletDistribution {
public:
	BulletDistribution() {
		m_dist.reserve(m_maxPosInspected);
	}

	inline bool isReading() const {
		return m_reading;
	}

	void addPosition(Vector const& pos, Vector const& emitterPosition) {
		if (isReading()) {
			throw Exception("Distribution in read mode.");
		}
		Scalar rotation = (pos - emitterPosition).getAngleOfSlope();
		m_dist.push_back({ rotation, 0 });
	}

	bool shouldReadNow() const {
		return !isReading() && (m_dist.size() >= m_maxPosInspected);
	}

	void beginReadingDistribution() {
		if (isReading()) {
			throw Exception("Distribution already in read mode.");
		}
		std::sort(m_dist.begin(), m_dist.end());
		int i, j;
		size_t sz = m_dist.size();
		int numOfBullets = 1;
		for (i = 0, j = sz - 1; i <= j; ++i, --j) {
			m_dist[i].second = numOfBullets + rand() % 2;
			m_dist[j].second = numOfBullets + rand() % 2;
			numOfBullets += 1;
		}
		m_reading = true;
		m_readIndex = 0;
	}

	std::pair<Scalar, int> const& operator[](size_t index) {
		if (!isReading()) {
			throw Exception("Distribution must be in read mode first.");
		}
		if (m_readIndex >= m_dist.size()) {
			throw Exception("Index out of range. No more distribution to read.");
		}
		return m_dist[index];
	}

	bool readNext(Scalar* rotationPtr, int* numOfBulletsPtr) {
		if (!isReading()) {
			throw Exception("Distribution must be in read mode first.");
		}
		if (m_readIndex >= m_dist.size()) {
			return false;
		}
		std::pair<Scalar, int> const& pair = m_dist[m_readIndex++];
		*rotationPtr = pair.first;
		*numOfBulletsPtr = pair.second;
		return true;
	}

	void endReadingDistribution() {
		if (!isReading()) {
			throw Exception("Distribution already in write mode.");
		}
		m_dist.clear();
		m_reading = false;
	}

private:
	size_t const m_maxPosInspected = 100;
	std::vector<std::pair<Scalar, int>> m_dist{};
	bool m_reading{ false };
	size_t m_readIndex{};
};

struct Bot {
	std::shared_ptr<BulletDistribution> bulletDistPtr;

	Bot()
		: bulletDistPtr(std::make_shared<BulletDistribution>()) {}
};

enum BulletEmitter {
	EMITTER_UNKNOWN = -1,
	EMITTER_PLAYER,
	EMITTER_BOT
};

struct Bullet {
	BulletEmitter emitter{ EMITTER_UNKNOWN };

	Bullet() = default;

	Bullet(BulletEmitter _emitter)
		: emitter(_emitter) {
	}
};

/////////////////////
////// SYSTEMS //////
/////////////////////

struct EntityCollisionAnalysisData {
	EntityID entityID = 0;
	Transform* tfPtr = nullptr;
	RigidBody const* rbPtr = nullptr; // since some colliders are not rigid bodies
	Collider* clPtr = nullptr; // areCollided() will not use this field
};

// TODO: Move this into CollisionSystem class (below).
class EntityCollisionDetector {
public:
	static bool areCollided(EntityCollisionAnalysisData const& data1, EntityCollisionAnalysisData const& data2) {
		Transform& tf1 = *(data1.tfPtr);
		Transform& tf2 = *(data2.tfPtr);
		RigidBody const& rb1 = *(data1.rbPtr);
		RigidBody const& rb2 = *(data2.rbPtr);
		Vector d = tf1.position - tf2.position;
		Scalar dModule = d.getModule();
		Scalar rSum = tf2.radius + tf1.radius;
		if (dModule < rSum) {
			// Collided
			return true;
			// TODO: Calculate collision impact (physical).
		}
		else {
			// Check tunneling
			return checkCollisionByTunneling(tf1.position, tf1.radius, rb1.velocity, tf2.position)
				|| checkCollisionByTunneling(tf2.position, tf2.radius, rb2.velocity, tf1.position);
		}
	}

	static bool checkCollisionByTunneling(Vector & pos, Scalar radius, Vector velocity, Vector posOther) {
		// Convention: uppercase: position of point (M, N, H) ; lowercase: vector (d).
		Vector const& M = pos;
		//Vector const  N = M - velocity;
		Vector const& d = velocity;
		Vector const& I = posOther;

		Scalar const dModule = d.getModule();
		if (dModule == 0) return false; // No displacement, no tunneling.

		// Find projection H of I on MN
		// H = M + t*d with t is a scalar. Using 2D geometry methods we have:
		Scalar t = (d.x * I.x + d.y * I.y - d.x * M.x - d.y * M.y) / dModule;
		Vector const H = M + t * d;

		// ((H - I).getModule() is the distance between I and line MN, which
		// will be less than entity1's radius if entity 1 has crossed entity2
		// during displacement.
		if ((H - I).getModule() < radius) {
			// Of course return true, but to avoid flicker when the entity is
			// misplaced, we modify the position a bit.
			pos = H; // where the collision takes place
			return true;
		}
		else return false;
	}
};

class CollisionSystem
	: public BaseSystem {
public:
	static Signature getSignature() {
		Signature sig;
		sig.enableComponent(gEcs.getComponentID<Transform>());
		sig.enableComponent(gEcs.getComponentID<Collider>());
		return sig;
	}

	void init() {

	}

	void quit() {

	}

	CollisionSystem()
		: m_rbMock{ new RigidBody(0, Vector(0, 0), Vector(0, 0)) } {

	}

	~CollisionSystem() {
		delete m_rbMock;
	}

	void update(Scalar dt) {
		std::vector<EntityCollisionAnalysisData> scannedEntities;
		EntityCollisionAnalysisData se;

		for (EntityID const& entityID : entityIDs) {
			Transform& tf = gEcs.getComponentDataOfEntityAsRef<Transform>(entityID);
			RigidBody const* rbPtr = nullptr;
			if (gEcs.entityHasComponent<RigidBody>(entityID)) {
				rbPtr = &( gEcs.getComponentDataOfEntityAsRef<RigidBody>(entityID) );
			} else {
				rbPtr = m_rbMock; // For entities without RigidBody component.
			}
			//RigidBody const& rb = *rbPtr;
			Collider& cl = gEcs.getComponentDataOfEntityAsRef<Collider>(entityID);
			cl.collidees.clear();
			se.entityID = entityID;
			se.tfPtr = &tf;
			se.rbPtr = rbPtr;
			se.clPtr = &cl;
			for (EntityCollisionAnalysisData const& scannedEntity : scannedEntities) {
				if (EntityCollisionDetector::areCollided(se, scannedEntity)) {
					cl.collidees.push_back(scannedEntity.entityID);
					scannedEntity.clPtr->collidees.push_back(entityID);
				}
			}
			scannedEntities.push_back(se);
		}
	}

private:
	RigidBody const* m_rbMock{ nullptr };
};

class KinematicsSystem
	: public BaseSystem {
public:
	static Signature getSignature() {
		Signature sig;
		sig.enableComponent(gEcs.getComponentID<Transform>());
		sig.enableComponent(gEcs.getComponentID<RigidBody>());
		return sig;
	}

	void init() {

	}

	void quit() {

	}

	void update(Scalar dt) {
		//auto const& reflectorArrayPtr = gEcs.getComponentDataArrayAsPtr<Reflector>();
		//size_t numReflectors = reflectorArrayPtr->getSize();

		std::set<EntityID> entitiesDeleteLater;
		for (EntityID const& entityID : entityIDs) {
			Transform& tf = gEcs.getComponentDataOfEntityAsRef<Transform>(entityID);
			RigidBody& rb = gEcs.getComponentDataOfEntityAsRef<RigidBody>(entityID);
			rb.velocity += rb.acceleration * dt;
			tf.position += rb.velocity * dt;

			// Process reflection the old way: Using Reflector component.
			/*for (size_t i = 0; i < numReflectors; ++i) {
				EntityID reflectorID;
				Reflector& Rrf = reflectorArrayPtr->accessByIndex(i, &reflectorID);
				Transform& Rtf = gEcs.getComponentDataOfEntityAsRef<Transform>(reflectorID);
				RigidBody Rrb(0, Vector(), Vector());
				EntityCollisionAnalysisData data1;
				{
					data1.entityID = entityID;
					data1.tfPtr = &tf;
					data1.rbPtr = &rb;
					data1.clPtr = nullptr;
				}
				EntityCollisionAnalysisData data2;
				{
					data2.entityID = reflectorID;
					data2.tfPtr = &Rtf;
					data2.rbPtr = &Rrb;
					data2.clPtr = nullptr;
				}
				if (EntityCollisionDetector::areCollided(data1, data2)) {
					// Reflect !
					rb.velocity.x *= Rrf.factors.x;
					rb.velocity.y *= Rrf.factors.y;
				}
			}*/
			if (tf.position.x >= gPlaygroundRect.w || tf.position.x <= 0 || tf.position.y >= gPlaygroundRect.h || tf.position.y <= 0) {
				entitiesDeleteLater.insert(entityID);
			}
		}

		for (EntityID const& e : entitiesDeleteLater) {
			gEcs.destroyEntity(e);
		}
	}
};

class DynamicsSystem
	: public BaseSystem {
public:
	static Signature getSignature() {
		Signature sig;
		sig.enableComponent(gEcs.getComponentID<RigidBody>());
		sig.enableComponent(gEcs.getComponentID<Collider>());
		return sig;
	}

	void init() {

	}

	void quit() {

	}

	void update(Scalar dt) {
		std::set<std::pair<int, int>> scannedPairs;
		for (EntityID const& entityID : entityIDs) {
			Collider& cl1 = gEcs.getComponentDataOfEntityAsRef<Collider>(entityID);
			RigidBody& rb1 = gEcs.getComponentDataOfEntityAsRef<RigidBody>(entityID);
			std::pair<int, int> scanningPair;
			for (EntityID const& entity2ID : cl1.collidees) {
				if (entityID == entity2ID) continue;
				if (!gEcs.entityHasComponent<RigidBody>(entity2ID)) continue; // some colliders are not rigid bodies
				else if (entityID < entity2ID) {
					scanningPair.first = entityID;
					scanningPair.second = entityID;
				} else {
					scanningPair.first = entity2ID;
					scanningPair.second = entityID;
				}
				if (scannedPairs.find(scanningPair) != scannedPairs.end()) continue; // already scanned
				RigidBody& rb2 = gEcs.getComponentDataOfEntityAsRef<RigidBody>(entity2ID);

				Vector const& v1 = rb1.velocity;
				Scalar const& m1 = rb1.mass;
				Vector const& v2 = rb2.velocity;
				Scalar const& m2 = rb2.mass;

				// https://vatlypt.com/va-cham-dan-hoi-va-cham-mem-bao-toan-dong-luong.html
				// Approximate: acceleration = delta(velocity) / dt
				Vector v1_ = ((m1 - m2) * v1 + 2 * m2 * v2) / (m1 + m2);
				Vector v2_ = (m1 * v1 + m2 * v2 - m1 * v1_) / m2;
				// Vector da1 = (v1_ - v1) / dt;
				// Vector da2 = (v2_ - v2) / dt;

				// Do not use acceleration since it's hard to reset it later ; the collision just causes an impulse.
				rb1.velocity = v1_; //rb1.acceleration += da1;
				rb2.velocity = v2_; //rb2.acceleration += da2;

				scannedPairs.insert(scanningPair);
			}
		}
	}
};

void createBullet(Vector const& position, Scalar gunAngle, BulletEmitter bulletEmitter) {
	Vector velocity(0.2, 0);
	velocity.rotate(gunAngle);

	EntityID entityID = gEcs.createEntity();
	gEcs.addComponentToEntity(entityID, Transform(position, Scalar(0), Scalar(5)));
	gEcs.addComponentToEntity(entityID, RigidBody(Scalar(20), velocity, Vector(0, 0)));
	gEcs.addComponentToEntity(entityID, Collider());
	gEcs.addComponentToEntity(entityID, Renderable());
	gEcs.addComponentToEntity(entityID, Bullet(bulletEmitter));
};

class PlayerSystem
	: public BaseSystem {
public:
	static Signature getSignature() {
		Signature sig;
		sig.enableComponent(gEcs.getComponentID<Player>());
		sig.enableComponent(gEcs.getComponentID<Transform>());
		sig.enableComponent(gEcs.getComponentID<Collider>());
		return sig;
	}

	PlayerSystem()
		: m_k(nullptr) {

	}

	void init() {
		// Keyboard state
		m_k = SDL_GetKeyboardState(NULL);
	}

	void quit() {

	}

	void update(Scalar dt) {
		if (entityIDs.empty()) {
			gGameRunning = false;
			return;
		}
		auto const& botArrayPtr = gEcs.getComponentDataArrayAsPtr<Bot>();

		std::set<EntityID> entitiesDeleteLater;

		for (EntityID const& playerID : entityIDs) {
			Transform& tf = gEcs.getComponentDataOfEntityAsRef<Transform>(playerID);
			// Player& pl = gEcs.getComponentDataOfEntityAsRef<Player>(playerID);
			Collider& cl = gEcs.getComponentDataOfEntityAsRef<Collider>(playerID);

			if (cl.isCollided()) {
				bool thisPlayerDied = false;
				for (EntityID entityID : cl.collidees) {
					if (gEcs.entityHasComponent<Bullet>(entityID)) {
						Bullet& bu = gEcs.getComponentDataOfEntityAsRef<Bullet>(entityID);
						if (bu.emitter == EMITTER_BOT) {
							entitiesDeleteLater.insert(playerID);
							thisPlayerDied = true;
							break;
						}
					}
				}
				if (thisPlayerDied) continue;
			}

			if (m_k[SDL_SCANCODE_SPACE]) {
				if (gBulletsLeft > 0) {
					createBullet(tf.position, tf.rotation, EMITTER_PLAYER);
					--gBulletsLeft;
				}
			}
#define MOVE_SPEED 0.3
			Scalar moveDelta = MOVE_SPEED * dt;
			Vector& position = tf.position;
			if (m_k[SDL_SCANCODE_UP]) {
				position.y -= moveDelta;
			}
			if (m_k[SDL_SCANCODE_DOWN]) {
				position.y += moveDelta;
			}
			if (m_k[SDL_SCANCODE_LEFT]) {
				position.x -= moveDelta;
			}
			if (m_k[SDL_SCANCODE_RIGHT]) {
				position.x += moveDelta;
			}

			Vector nearestBotPosition = findPositionOfNearestBot(position, botArrayPtr);
			tf.rotation = (nearestBotPosition - position).getAngleOfSlope();
		}

		for (EntityID const& e : entitiesDeleteLater) {
			gEcs.destroyEntity(e);
		}
	}

	Vector findPositionOfNearestBot(Vector const& playerPosition, std::shared_ptr<ComponentDataArray<Bot>> const& botArrayPtr) {
		Scalar nearestDistance = 0;
		//EntityID nearestID = 0;
		Vector nearestPosition{};

		size_t const numOfBots = botArrayPtr->getSize();
		for (size_t i = 0; i < numOfBots; ++i) {
			EntityID botID;
			botArrayPtr->accessByIndex(i, &botID);
			Vector botPosition = gEcs.getComponentDataOfEntityAsRef<Transform>(botID).position;
			Scalar botDistance = (botPosition - playerPosition).getModule();
			if (i == 0 || botDistance < nearestDistance) {
				nearestDistance = botDistance;
				//nearestID = botID;
				nearestPosition = botPosition;
			}
		}

		return nearestPosition;
	}

private:
	Uint8 const* m_k;
};

class BotSystem
	: public BaseSystem {
public:
	static Signature getSignature() {
		Signature sig;
		sig.enableComponent(gEcs.getComponentID<Bot>());
		sig.enableComponent(gEcs.getComponentID<Transform>());
		return sig;
	}

	void init() {

	}

	void quit() {

	}

	void update(Scalar dt) {
		auto const& playerArrayPtr = gEcs.getComponentDataArrayAsPtr<Player>();
		size_t numOfPlayers = playerArrayPtr->getSize();

for (EntityID botID : entityIDs) {
	Bot& bo = gEcs.getComponentDataOfEntityAsRef<Bot>(botID);
	Transform& Btf = gEcs.getComponentDataOfEntityAsRef<Transform>(botID);
	BulletDistribution& bulletDist = *(bo.bulletDistPtr);

	for (size_t i = 0; i < numOfPlayers; ++i) {
		EntityID playerID;
		playerArrayPtr->accessByIndex(i, &playerID);
		Transform& Ptf = gEcs.getComponentDataOfEntityAsRef<Transform>(playerID);

		if (!bulletDist.isReading()) {
			if (rand()) bulletDist.addPosition(Ptf.position, Btf.position);
		}
	}

	if (bulletDist.shouldReadNow()) {
		bulletDist.beginReadingDistribution();
	}
	if (bulletDist.isReading()) {
		Scalar rotation{};
		int numOfBullets{};
		if (!bulletDist.readNext(&rotation, &numOfBullets)) {
			bulletDist.endReadingDistribution();
		}
		else {
			Btf.rotation += rotation;
			if (rand()) createBullet(Btf.position, Btf.rotation, EMITTER_BOT);
		}
	}
}
	}
};

std::string formatAge(Scalar age) {
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

class RenderSystem
	: public BaseSystem {
public:
	static Signature getSignature() {
		Signature sig;
		sig.enableComponent(gEcs.getComponentID<Transform>());
		sig.enableComponent(gEcs.getComponentID<Renderable>());
		return sig;
	}

	RenderSystem()
		: m_renderer(NULL), m_digiFont(NULL) {

	}

	void init(SDL_Renderer* renderer) {
		m_renderer = renderer;
		m_digiFont = TTF_OpenFont("res/fonts/DS-DIGI.ttf", gStatusAreaRect.h - 1);
		if (m_digiFont == NULL) {
			throw Exception("Could not load font.");
		}
	}

	void quit() {
		TTF_CloseFont(m_digiFont);
	}

	enum Alignment {
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER,
		ALIGN_TOP,
		ALIGN_BOTTOM
	};
	SDL_Rect drawText(Vector position, std::string const& s, TTF_Font* const font, SDL_Color const& fontColor, Alignment horizontalAlign = ALIGN_LEFT, Alignment verticalAlign = ALIGN_TOP) {
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
		SDL_FreeSurface(fontSf);
		return fontDstRect;
	}

	void update() {
		SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
		SDL_RenderClear(m_renderer);

		SDL_RenderSetViewport(m_renderer, &gStatusAreaRect);
		std::string sTime = formatAge(gAge);
		SDL_Color fontColor = { 0, 255, 0, 255 };
		if (!gGameRunning) {
			fontColor.r = 255;
			fontColor.g = 0;
			fontColor.b = 0;
			fontColor.a = 255;
		}
		{
			drawText(Vector(gStatusAreaRect.x + 1, gStatusAreaRect.y + 1), sTime, m_digiFont, fontColor);

			if (gGameRunning) {
				fontColor.r = 0;
				fontColor.g = 0;
				fontColor.b = 255;
				int a = 128 + gLevel * 10;
				if (a >= 256) a = 255;
				fontColor.a = static_cast<Uint8>(a);
			}
			drawText(Vector(gStatusAreaRect.x + gStatusAreaRect.w - 3, gStatusAreaRect.y + 1), std::to_string(gBulletsLeft), m_digiFont, fontColor, ALIGN_RIGHT);
		}

		SDL_RenderSetViewport(m_renderer, &gPlaygroundRect);
		for (EntityID const& entityID : entityIDs) {
			Transform& tf = gEcs.getComponentDataOfEntityAsRef<Transform>(entityID);
			// ATTENTION: Will use this variable in the near future
			// Renderable& rd = gEcs.getComponentDataOfEntityAsRef<Renderable>(entityID);
			if (gEcs.entityHasComponent<Bullet>(entityID)) {
				Bullet& bl = gEcs.getComponentDataOfEntityAsRef<Bullet>(entityID);
				if (bl.emitter == EMITTER_PLAYER) {
					SDL_SetRenderDrawColor(m_renderer, 46, 140, 255, 255);
				}
				else if (bl.emitter == EMITTER_BOT) {
					SDL_SetRenderDrawColor(m_renderer, 127, 10, 10, 255);
				}
			}
			if (gEcs.entityHasComponent<Player>(entityID)) {
				//Player& pl = gEcs.getComponentDataOfEntityAsRef<Player>(entityID);
				Vector direction(tf.radius, 0);
				direction.rotate(tf.rotation);
				Vector topLeft = tf.position - direction;
				Vector bottomRight = tf.position + direction;
				SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255);
				SDL_RenderDrawLine(m_renderer, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
				SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 255);
			}
			if (gEcs.entityHasComponent<Bot>(entityID)) {
				//Bot& bo = gEcs.getComponentDataOfEntityAsRef<Bot>(entityID);
				Vector direction(tf.radius, 0);
				direction.rotate(tf.rotation);
				Vector topLeft = tf.position - direction;
				Vector bottomRight = tf.position + direction;
				SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255);
				SDL_RenderDrawLine(m_renderer, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
				SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255); // TODO: Clean this duplicating code
			}
			SDL_RenderFillCircle(m_renderer, tf.position.x, tf.position.y, tf.radius);
		}

		SDL_RenderSetViewport(m_renderer, NULL);

		if (!gGameRunning) {
			SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 127);
			SDL_RenderFillRect(m_renderer, NULL);

			fontColor.r = 255;
			fontColor.g = 255;
			fontColor.b = 255;
			fontColor.a = 255;
#define drawTextGameOver() drawText(Vector(gWindowWidth, gWindowHeight) / 2, "GAME OVER", m_digiFont, fontColor, ALIGN_CENTER, ALIGN_CENTER)
			SDL_Rect r = drawTextGameOver();
			SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(m_renderer, &r);
			drawTextGameOver();
		}

		SDL_RenderPresent(m_renderer);
	}

private:
	SDL_Renderer* m_renderer;
	TTF_Font* m_digiFont;
};

extern "C" int main(int argc, char* argv[]) {
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			return -1;
		}

		int img = IMG_INIT_PNG; // SDL_image from vcpkg does not support JPEG (2023 Apr 08)
		if ((IMG_Init(img) & img) != img) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, IMG_GetError());
			return -1;
		}

		if (TTF_Init() != 0) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, TTF_GetError());
			return -1;
		}

		int mix = MIX_INIT_MP3 | MIX_INIT_MID;
		if ((Mix_Init(mix) & mix) != mix) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, Mix_GetError());
			return -1;
		}
	}
	SDL_Window* window = SDL_CreateWindow("Fireback", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gWindowWidth, gWindowHeight, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		return -1;
	}
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		return -1;
	}

	gEcs.init();

	gEcs.registerComponent<Transform>();
	gEcs.registerComponent<RigidBody>();
	gEcs.registerComponent<Collider>();
	//gEcs.registerComponent<Reflector>();
	gEcs.registerComponent<Renderable>();
	gEcs.registerComponent<Player>();
	gEcs.registerComponent<Bot>();
	gEcs.registerComponent<Bullet>();

	auto rend = gEcs.registerSystem<RenderSystem>(RenderSystem::getSignature());
	rend->init(renderer);

	auto col = gEcs.registerSystem<CollisionSystem>(CollisionSystem::getSignature());
	col->init();

	auto kin = gEcs.registerSystem<KinematicsSystem>(KinematicsSystem::getSignature());
	kin->init();

	auto dyn = gEcs.registerSystem<DynamicsSystem>(DynamicsSystem::getSignature());
	dyn->init();

	auto pls = gEcs.registerSystem<PlayerSystem>(PlayerSystem::getSignature());
	pls->init();

	auto bos = gEcs.registerSystem<BotSystem>(BotSystem::getSignature());
	bos->init();

	EntityID playerID = gEcs.createEntity();
	{
		gEcs.addComponentToEntity(playerID, Player());
		gEcs.addComponentToEntity(playerID, Transform(Vector(20, 20), 0, 10));
		gEcs.addComponentToEntity(playerID, Collider());
		gEcs.addComponentToEntity(playerID, Renderable());
	}

	EntityID botID = gEcs.createEntity();
	{
		gEcs.addComponentToEntity(botID, Bot());
		gEcs.addComponentToEntity(botID, Transform(Vector(gWindowWidth / 2, gWindowHeight / 2), 0, 15));
		gEcs.addComponentToEntity(botID, Renderable());
	}

	// TTF_Font*

	SDL_Event event;
	Scalar dt = 0;

	bool running = true;
	gGameRunning = true;

	while (running && gGameRunning) {
		auto startTime = std::chrono::high_resolution_clock::now();

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				gGameRunning = false;
				break;
			}
		}

		rend->update();

		kin->update(dt);
		col->update(dt);
		dyn->update(dt);

		pls->update(dt);
		bos->update(dt);

		if (gAge - gLastAgeMerit >= 20000 * gLevel) {
			gBulletsLeft += gLevel * 3000;
			++gLevel;
			gLastAgeMerit = gAge;
		}

		auto stopTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration<Scalar, std::chrono::milliseconds::period>(stopTime - startTime);
		dt = duration.count();
		gAge += dt;
	}

	rend->update();
	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_RETURN:
					running = false;
					break;
				}
			}
		}
	}

	kin->quit();
	col->quit();
	dyn->quit();
	pls->quit();
	bos->quit();
	rend->quit();

	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}
