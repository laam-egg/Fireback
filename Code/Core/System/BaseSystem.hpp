#ifndef BaseSystem_INCLUDED
#define BaseSystem_INCLUDED

#include <set>
#include "Core/Entity/EntityID.hpp"

class BaseSystem {
public:
	std::set<EntityID> entityIDs;
	// This set is managed by SystemManager.
};

#endif // BaseSystem_INCLUDED
