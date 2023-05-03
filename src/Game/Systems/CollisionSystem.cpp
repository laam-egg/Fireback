#include "Game/Systems/CollisionSystem.hpp"
#include "Game/Game.hpp"

Signature CollisionSystem::getSignature() {
	Signature sig;
	sig.enableComponent(G.ecs.getComponentID<Transform>());
	sig.enableComponent(G.ecs.getComponentID<Collider>());
	return sig;
}

struct EntityCollisionAnalysisData {
	EntityID entityID = 0;
	Transform* tfPtr = nullptr;
	RigidBody const* rbPtr = nullptr; // since some colliders are not rigid bodies
	Collider* clPtr = nullptr; // areCollided() will not use this field
};

// TODO: Move this into CollisionSystem class (below).
class EntityCollisionDetector {
public:
	static bool areCollided(EntityCollisionAnalysisData const& data1, EntityCollisionAnalysisData const& data2, Vector* collisionPositionPtr = nullptr) {
		Transform& tf1 = *(data1.tfPtr);
		Transform& tf2 = *(data2.tfPtr);
		RigidBody const& rb1 = *(data1.rbPtr);
		RigidBody const& rb2 = *(data2.rbPtr);
		Scalar dModule = (tf1.position - tf2.position).getModule();
		Scalar rSum = tf2.radius + tf1.radius;
		if (dModule < rSum) {
			// Collided. The collision point is approximately the midpoint of line segment from position1 to position2.
			if (collisionPositionPtr != nullptr) {
				*collisionPositionPtr = (tf1.position + tf2.position) / 2;
			}
			return true;
			// TODO: Calculate collision impact (physical).
		}
		else {
			// Check tunneling
			return checkCollisionByTunnelingOfBothThroughEachOther(
				tf1.position, rb1.velocity, tf1.radius,
				tf2.position, rb2.velocity, tf2.radius,
				collisionPositionPtr
			);
		}
	}

	static bool checkCollisionByTunnelingOfBothThroughEachOther(
		Vector const& pos, Vector const& velocity, Scalar const& radius,
		Vector const& posOther, Vector const& velocityOther, Scalar const& radiusOther,
		Vector* collisionPositionPtr = nullptr
	) {
		if (velocity.isZeroVector()) {
			if (velocityOther.isZeroVector()) {
				// Both are zero vectors => no tunneling.
				return false;
			}
			else {
				// v1 is zero but v2 is not.
				return checkCollisionByTunnelingOfThisThroughOther(posOther, velocityOther, pos, radius, collisionPositionPtr);
			}
		}
		else {
			if (velocityOther.isZeroVector()) {
				// v2 is zero but v1 is not.
				return checkCollisionByTunnelingOfThisThroughOther(pos, velocity, posOther, radiusOther, collisionPositionPtr);
			}
			else {
				// Neither is zero vector => check crossing.
				Scalar a1 = velocity.getSlope();
				Scalar a2 = velocity.getSlope();
				Scalar b1 = pos.y - a1 * pos.x;
				Scalar b2 = posOther.y - a2 * posOther.x;
				// So the movements of the two entities form two lines, y = a1*x + b1 and y = a2*x + b2, respectively.
				// Now we find the intersection point of these two lines.
				Vector const  A = pos - velocity;
				Vector const& B = pos;
				Vector const  C = posOther - velocityOther;
				Vector const& D = posOther;
				if (a2 == a1) {
					if (b2 == b1) {
						// The two lines overlap ; but do the two line SEGMENTS overlap ?
						// Segment AB overlaps segment CD only if AB + BC + CD > AD.
						if (abs(A - B) + abs(B - C) + abs(C - D) > abs(A - D)) {
							// Collide. Take the midpoint of segment BC as the collision point.
							if (collisionPositionPtr != nullptr) {
								*collisionPositionPtr = (B + C) / 2;
							}
							return true;
						}
						else return false;
					}
					else {
						// The two lines are parallel with each other.
						return false;
					}
				}
				else {
					// The two entities are moving in different directions.
					// The intersection point of the two lines AB and CD is M(x, y) where:
					Scalar x = (b2 - b1) / (a2 - a1);
					Scalar y = a1 * x + b1;
					Vector M(x, y);
					// Check if (x, y) is WITHIN both SEGMENTS AB and CD.
					// Instead of: if (abs(A - M) <= abs(A - B) && abs(C - M) <= abs(C - D)) {
					// More efficient, I guess:
					if (x >= A.x && y >= A.y && x <= B.x && y <= B.y
						&& x >= C.x && y >= C.y && x <= D.x && y <= D.y) {
						// Yes, they are within. Collided.
						if (collisionPositionPtr != nullptr) {
							*collisionPositionPtr = M;
						}
						return true;
					}
					else {
						// They have yet reached each other.
						return false;
					}
				}
			}
		}
	}

	static bool checkCollisionByTunnelingOfThisThroughOther(Vector const& pos, Vector const& velocity, Vector const& posOther, Scalar const& radiusOther, Vector* collisionPositionPtr = nullptr) {
		// Convention: uppercase: position of point (M, N, H) ; lowercase: vector (d).
		Vector const& M = pos;
		//Vector const  N = M - velocity;
		Vector const& d = velocity; // vector MN: displacement vector of entity 1
		Vector const& I = posOther;

		Scalar const dModule = d.getModule();
		if (dModule == 0) return false; // No displacement, no tunneling.

		// Find projection H of I on MN
		// H = M + t*d with t is a scalar. Using 2D geometry methods we have:
		Scalar t = (d.x * I.x + d.y * I.y - d.x * M.x - d.y * M.y) / dModule;
		Vector const H = M + t * d;

		// ((H - I).getModule() is the distance between I and line MN, which
		// will be less than entity2's radius if entity 1 has crossed entity2
		// during displacement.
		if ((H - I).getModule() < radiusOther) {
			if (collisionPositionPtr != nullptr) {
				*collisionPositionPtr = H;
			}
			return true;
		}
		else return false;
	}

};

void CollisionSystem::init() {

}

void CollisionSystem::quit() {

}

CollisionSystem::CollisionSystem()
	: m_rbMock{ new RigidBody(0, Vector(0, 0), Vector(0, 0)) } {

}

CollisionSystem::~CollisionSystem() {
	delete m_rbMock;
}

void CollisionSystem::update(Scalar dt) {
	std::vector<EntityCollisionAnalysisData> scannedEntities;
	EntityCollisionAnalysisData se;

	for (EntityID entityID : entityIDs) {
		Transform& tf = G.ecs.getComponentDataOfEntityAsRef<Transform>(entityID);
		RigidBody const* rbPtr = nullptr;
		if (G.ecs.entityHasComponent<RigidBody>(entityID)) {
			rbPtr = &(G.ecs.getComponentDataOfEntityAsRef<RigidBody>(entityID));
		}
		else {
			rbPtr = m_rbMock; // For entities without RigidBody component.
		}
		//RigidBody const& rb = *rbPtr;
		Collider& cl = G.ecs.getComponentDataOfEntityAsRef<Collider>(entityID);
		cl.collidees.clear();
		se.entityID = entityID;
		se.tfPtr = &tf;
		se.rbPtr = rbPtr;
		se.clPtr = &cl;
		Vector collisionPosition{};
		for (EntityCollisionAnalysisData const& scannedEntity : scannedEntities) {
			if (EntityCollisionDetector::areCollided(se, scannedEntity, &collisionPosition)) {
				cl.collidees.push_back({ scannedEntity.entityID, collisionPosition });
				scannedEntity.clPtr->collidees.push_back({ entityID, collisionPosition });
			}
		}
		scannedEntities.push_back(se);
	}
}
