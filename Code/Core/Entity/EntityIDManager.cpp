#include "Core/Entity/EntityIDManager.hpp"
#include "Core/Exception.hpp"

EntityIDManager::EntityIDManager()
    : m_unusedEntityIDs{},
    m_entitySignatures{},
    m_livingEntityCount(0) {
    for (EntityID id = 0; id <= MAX_ENTITY_ID; ++id) {
        m_unusedEntityIDs.push(id);
    }
}

EntityID EntityIDManager::acquireEntityID() {
    if (m_livingEntityCount > MAX_ENTITIES) {
        throw Exception("No more entity ID available.");
    }
    EntityID id = m_unusedEntityIDs.front();
    m_unusedEntityIDs.pop();
    ++m_livingEntityCount;
    return id;
}

void EntityIDManager::releaseEntityID(EntityID id) {
    checkEntityIDValidity(id);
    m_entitySignatures[id].reset();
    m_unusedEntityIDs.push(id);
    --m_livingEntityCount;
}

void EntityIDManager::setEntitySignature(EntityID id, Signature sig) {
    checkEntityIDValidity(id);
    m_entitySignatures[id] = sig;
}

Signature const& EntityIDManager::getEntitySignature(EntityID id) {
    checkEntityIDValidity(id);
    return m_entitySignatures[id];
}

inline void EntityIDManager::checkEntityIDValidity(EntityID id) {
    if (id > MAX_ENTITY_ID) {
        throw Exception("Invalid entity ID.");
    }
    // TODO: Check if id is present in the queue ; but that may be unnecessary.
}
