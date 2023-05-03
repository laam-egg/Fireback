#ifndef Collider_COMPONENT_INCLUDED
#define Collider_COMPONENT_INCLUDED

#include <vector>
#include "Core/Vector.hpp"
#include "Core/Entity/EntityID.hpp"

struct CollideeInfo {
	EntityID entityID;
	Vector collisionPosition;
};

struct Collider {
	std::vector<CollideeInfo> collidees{};

	Collider() = default;

	inline bool isCollided() const {
		return !collidees.empty();
	}
};

#endif // Collider_COMPONENT_INCLUDED
