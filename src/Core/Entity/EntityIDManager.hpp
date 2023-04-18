#ifndef EntityIDManager_INCLUDED
#define EntityIDManager_INCLUDED

#include <queue>
#include <array>
#include "Core/Entity/EntityID.hpp"
#include "Core/ECS/Signature.hpp"

/**
 * This class is in charge of managing used and unused
 * entity IDs.
 */
class EntityIDManager {
public:
    EntityIDManager();

    EntityID acquireEntityID();

    void releaseEntityID(EntityID id);

    void setEntitySignature(EntityID id, Signature sig);

    Signature const& getEntitySignature(EntityID id);

	void restart();

private:
    void checkEntityIDValidity(EntityID id);

	void resetUnusedEntityIDs();

	std::queue<EntityID> m_unusedEntityIDs{};
	std::array<Signature, MAX_ENTITIES> m_entitySignatures{};
	EntityIDCount m_livingEntityCount{};
};

#endif // EntityIDManager_INCLUDED
