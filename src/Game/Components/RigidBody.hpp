#ifndef RigidBody_COMPONENT_INCLUDED
#define RigidBody_COMPONENT_INCLUDED

#include "Core/Vector.hpp"

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

#endif // RigidBody_COMPONENT_INCLUDED
