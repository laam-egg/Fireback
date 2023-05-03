#include "Game/Systems/PlayerSystem.hpp"
#include "Game/Game.hpp"

Signature PlayerSystem::getSignature() {
	Signature sig;
	sig.enableComponent(G.ecs.getComponentID<Player>());
	sig.enableComponent(G.ecs.getComponentID<Transform>());
	sig.enableComponent(G.ecs.getComponentID<Collider>());
	return sig;
}

PlayerSystem::PlayerSystem()
	: m_k(nullptr) {

}

void PlayerSystem::init() {
	// Keyboard state
	m_k = SDL_GetKeyboardState(NULL);
}

void PlayerSystem::quit() {

}

void PlayerSystem::update(Scalar dt) {
	if (entityIDs.empty()) {
		G.gamePlaying = false;
		return;
	}
	auto const& botArrayPtr = G.ecs.getComponentDataArrayAsPtr<Bot>();

	std::set<EntityID> entitiesDeleteLater;

	for (EntityID const& playerID : entityIDs) {
		Transform& tf = G.ecs.getComponentDataOfEntityAsRef<Transform>(playerID);
		// Player& pl = G.ecs.getComponentDataOfEntityAsRef<Player>(playerID);
		Collider& cl = G.ecs.getComponentDataOfEntityAsRef<Collider>(playerID);

		if (cl.isCollided()) {
			bool thisPlayerDied = false;
			for (CollideeInfo const& collideeInfo : cl.collidees) {
				EntityID const& entityID = collideeInfo.entityID;
				if (G.ecs.entityHasComponent<Bullet>(entityID)) {
					Bullet& bu = G.ecs.getComponentDataOfEntityAsRef<Bullet>(entityID);
					if (bu.emitter == EMITTER_BOT) {
						// Player killed
						G.createFire(collideeInfo.collisionPosition);
						entitiesDeleteLater.insert(playerID);
						thisPlayerDied = true;
						break;
					}
				}
			}
			if (thisPlayerDied) continue;
		}

		if (m_k[SDL_SCANCODE_SPACE]) {
			if (G.bulletsLeft > 0) {
				G.createBullet(tf.position, tf.rotation, EMITTER_PLAYER);
				--G.bulletsLeft;
			}
		}
#define MOVE_SPEED 0.3f
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

		Scalar const maxPosition_x = G.playgroundRect.w - tf.radius; // + G.playgroundRect.x; // RenderSystem already takes care of the viewport, so the .x advance is not needed.
		Scalar const maxPosition_y = G.playgroundRect.h - tf.radius;
		Scalar const minPosition_x = 0 + tf.radius;
		Scalar const minPosition_y = 0 + tf.radius;
#define LIMIT(coord) \
if (position.coord <= minPosition_##coord) position.coord = minPosition_##coord; \
else if (position.coord >= maxPosition_##coord) position.coord = maxPosition_##coord

		LIMIT(x);
		LIMIT(y);
#undef LIMIT

		Vector nearestBotPosition = findPositionOfNearestBot(position, botArrayPtr);
		tf.rotation = (nearestBotPosition - position).getAngleOfSlope();
	}

	for (EntityID const& e : entitiesDeleteLater) {
		G.ecs.destroyEntity(e);
	}
}

Vector PlayerSystem::findPositionOfNearestBot(Vector const& playerPosition, std::shared_ptr<ComponentDataArray<Bot>> const& botArrayPtr) {
	Scalar nearestDistance = 0;
	//EntityID nearestID = 0;
	Vector nearestPosition{};

	size_t const numOfBots = botArrayPtr->getSize();
	for (size_t i = 0; i < numOfBots; ++i) {
		EntityID botID;
		botArrayPtr->accessByIndex(i, &botID);
		Vector botPosition = G.ecs.getComponentDataOfEntityAsRef<Transform>(botID).position;
		Scalar botDistance = (botPosition - playerPosition).getModule();
		if (i == 0 || botDistance < nearestDistance) {
			nearestDistance = botDistance;
			//nearestID = botID;
			nearestPosition = botPosition;
		}
	}

	return nearestPosition;
}
