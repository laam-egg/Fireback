#include "Game/Systems/KinematicsSystem.hpp"
#include "Game/Game.hpp"

Signature KinematicsSystem::getSignature() {
	Signature sig;
	sig.enableComponent(G.ecs.getComponentID<Transform>());
	sig.enableComponent(G.ecs.getComponentID<RigidBody>());
	return sig;
}

void KinematicsSystem::init() {

}

void KinematicsSystem::quit() {

}

void KinematicsSystem::update(Scalar dt) {
	//auto const& reflectorArrayPtr = G.ecs.getComponentDataArrayAsPtr<Reflector>();
	//size_t numReflectors = reflectorArrayPtr->getSize();

	std::set<EntityID> entitiesDeleteLater;
	for (EntityID const& entityID : entityIDs) {
		Transform& tf = G.ecs.getComponentDataOfEntityAsRef<Transform>(entityID);
		RigidBody& rb = G.ecs.getComponentDataOfEntityAsRef<RigidBody>(entityID);
		rb.velocity += rb.acceleration * dt;
		tf.position += rb.velocity * dt;

		// Process reflection the old way: Using Reflector component.
		/*for (size_t i = 0; i < numReflectors; ++i) {
			EntityID reflectorID;
			Reflector& Rrf = reflectorArrayPtr->accessByIndex(i, &reflectorID);
			Transform& Rtf = G.ecs.getComponentDataOfEntityAsRef<Transform>(reflectorID);
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
		if (tf.position.x >= G.playgroundRect.w || tf.position.x <= 0 || tf.position.y >= G.playgroundRect.h || tf.position.y <= 0) {
			entitiesDeleteLater.insert(entityID);
		}
	}

	for (EntityID const& e : entitiesDeleteLater) {
		G.ecs.destroyEntity(e);
	}
}
