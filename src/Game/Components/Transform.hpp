#ifndef Transform_COMPONENT_INCLUDED
#define Transform_COMPONENT_INCLUDED

#include "Core/Vector.hpp"

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

#endif // Transform_COMPONENT_INCLUDED
