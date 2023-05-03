#ifndef ComponentManager_IMPL_INCLUDED
#define ComponentManager_IMPL_INCLUDED

#include "Core/Component/ComponentManager.hpp"

//////////////////////////
// BEGIN IMPLEMENTATION //
//////////////////////////

#include "Core/Exception.hpp"

void ComponentManager::restart() {
	for (auto const& pair : m_componentDataArrays) {
		auto const& dataArrayPtr = pair.second;
		dataArrayPtr->restart();
	}
}

ComponentManager::ComponentManager() {
	// restart();
}

template<typename T>
ComponentID ComponentManager::registerComponent() {
	ComponentLookupKey k = getLookupKeyOfAnyComponentType<T>();
	if (isComponentRegistered(k)) {
		throw Exception("Component already registered.");
	}
	m_componentIDs[k] = m_nextComponentID;
	m_componentDataArrays[k] = std::make_shared<ComponentDataArray<T>>();
	return m_nextComponentID++;
}

template<typename T>
void ComponentManager::addComponentToEntity(EntityID entityID, T componentData) {
	return getComponentDataArrayAsPtr<T>()->addComponentToEntity(entityID, componentData);
}

template<typename T>
void ComponentManager::removeComponentFromEntity(EntityID entityID) {
	return getComponentDataArrayAsPtr<T>()->removeComponentFromEntity(entityID);
}

template<typename T>
ComponentID ComponentManager::getComponentID() {
	ComponentLookupKey k = getLookupKeyOfAnyComponentType<T>();
	auto const& iter = m_componentIDs.find(k);
	if (iter == m_componentIDs.end()) {
		throw Exception("No component with such ID registered.");
	}
	return iter->second;
}

template<typename T>
T& ComponentManager::getComponentDataOfEntityAsRef(EntityID entityID) {
	return getComponentDataArrayAsPtr<T>()->getComponentDataOfEntityAsRef(entityID);
}

void ComponentManager::onEntityDestroyed(EntityID entityID) {
	for (auto const& pair : m_componentDataArrays) {
		auto const& componentDataArrayAsPtr = pair.second;
		componentDataArrayAsPtr->onEntityDestroyed(entityID);
	}
}

template<typename T>
std::shared_ptr<ComponentDataArray<T>> ComponentManager::getComponentDataArrayAsPtr() const {
	ComponentLookupKey k = getLookupKeyOfAnyComponentType<T>();
	auto const& componentDataArraysIter = m_componentDataArrays.find(k);
	if (componentDataArraysIter == m_componentDataArrays.end()) {
		throw Exception("Component not yet registered.");
	}
	return std::static_pointer_cast<ComponentDataArray<T>>(componentDataArraysIter->second);
}

inline bool ComponentManager::isComponentRegistered(ComponentLookupKey k) {
	return m_componentIDs.find(k) != m_componentIDs.end();
}

inline void ComponentManager::ensureComponentRegistered(ComponentLookupKey k) {
	if (!isComponentRegistered(k)) {
		throw Exception("Component not yet registered.");
	}
}

///////////////////////////
// END OF IMPLEMENTATION //
///////////////////////////

#define ComponentManager_INSTANTIATE_TEMPLATE_FOR_COMPONENT(T) \
	template ComponentID ComponentManager::registerComponent<T>(); \
	template void ComponentManager::addComponentToEntity<T>(EntityID entityID, T componentData); \
	template void ComponentManager::removeComponentFromEntity<T>(EntityID entityID); \
	template ComponentID ComponentManager::getComponentID<T>(); \
	template T& ComponentManager::getComponentDataOfEntityAsRef<T>(EntityID entityID); \
	template std::shared_ptr<ComponentDataArray<T>> ComponentManager::getComponentDataArrayAsPtr<T>() const; \
	template ComponentLookupKey ComponentManager::getLookupKeyOfAnyComponentType<T>()

#endif // ComponentManager_IMPL_INCLUDED
