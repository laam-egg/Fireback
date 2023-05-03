#include "Game/Systems/BotSystem.hpp"
#include "Game/Game.hpp"

Signature BotSystem::getSignature() {
	Signature sig;
	sig.enableComponent(G.ecs.getComponentID<Bot>());
	sig.enableComponent(G.ecs.getComponentID<Transform>());
	return sig;
}

void BotSystem::init() {

}

void BotSystem::quit() {

}

void BotSystem::update(Scalar dt) {
	auto const& playerArrayPtr = G.ecs.getComponentDataArrayAsPtr<Player>();
	size_t numOfPlayers = playerArrayPtr->getSize();

	for (EntityID botID : entityIDs) {
		Bot& bo = G.ecs.getComponentDataOfEntityAsRef<Bot>(botID);
		Transform& Btf = G.ecs.getComponentDataOfEntityAsRef<Transform>(botID);
		BulletDistribution& bulletDist = *(bo.bulletDistPtr);

		for (size_t i = 0; i < numOfPlayers; ++i) {
			EntityID playerID;
			playerArrayPtr->accessByIndex(i, &playerID);
			Transform& Ptf = G.ecs.getComponentDataOfEntityAsRef<Transform>(playerID);

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
				if (rand()) G.createBullet(Btf.position, Btf.rotation, EMITTER_BOT);
			}
		}
	}
}
