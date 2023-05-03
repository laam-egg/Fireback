#include "Game/Systems/DynamicsSystem.hpp"
#include "Game/Game.hpp"

Signature DynamicsSystem::getSignature() {
	Signature sig;
	sig.enableComponent(G.ecs.getComponentID<RigidBody>());
	sig.enableComponent(G.ecs.getComponentID<Collider>());
	return sig;
}

void DynamicsSystem::init() {

}

void DynamicsSystem::quit() {

}

#include <set>

void DynamicsSystem::update(Scalar dt) {
	std::set<std::pair<int, int>> scannedPairs;
	for (EntityID const& entityID : entityIDs) {
		Collider& cl1 = G.ecs.getComponentDataOfEntityAsRef<Collider>(entityID);
		RigidBody& rb1 = G.ecs.getComponentDataOfEntityAsRef<RigidBody>(entityID);
		std::pair<int, int> scanningPair;
		for (CollideeInfo const& collideeInfo : cl1.collidees) {
			EntityID const& entity2ID = collideeInfo.entityID;
			if (entityID == entity2ID) continue;
			if (!G.ecs.entityHasComponent<RigidBody>(entity2ID)) continue; // some colliders are not rigid bodies
			else if (entityID < entity2ID) {
				scanningPair.first = entityID;
				scanningPair.second = entityID;
			}
			else {
				scanningPair.first = entity2ID;
				scanningPair.second = entityID;
			}
			if (scannedPairs.find(scanningPair) != scannedPairs.end()) continue; // already scanned
			RigidBody& rb2 = G.ecs.getComponentDataOfEntityAsRef<RigidBody>(entity2ID);

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
