#ifndef ComponentManager_INCLUDED
#define ComponentManager_INCLUDED

#include <unordered_map>
#include <memory>
#include "Core/Component/ComponentDataArray.hpp"

using ComponentLookupKey = char const*;

/**
 * This class takes care of adding or removing components of
 * all types.
 */
class ComponentManager {
public:
	ComponentManager();

	template<typename T>
	ComponentID registerComponent();

	template<typename T>
	void addComponentToEntity(EntityID entityID, T componentData);

	template<typename T>
	void removeComponentFromEntity(EntityID entityID);

	template<typename T>
	ComponentID getComponentID();

	template<typename T>
	T& getComponentDataOfEntityAsRef(EntityID entityID);

	void onEntityDestroyed(EntityID id);

	template<typename T>
	std::shared_ptr<ComponentDataArray<T>> getComponentDataArrayAsPtr() const;

	void restart();

private:
	std::unordered_map<ComponentLookupKey, ComponentID> m_componentIDs{};
	std::unordered_map<ComponentLookupKey, std::shared_ptr<AbstractComponentDataArray>> m_componentDataArrays{};
	ComponentID m_nextComponentID{};

	template<typename T>
	static inline ComponentLookupKey getLookupKeyOfAnyComponentType() {
		return typeid(T).name();
	}

	bool isComponentRegistered(ComponentLookupKey k);

	void ensureComponentRegistered(ComponentLookupKey k);
};


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

#endif // ComponentManager_INCLUDED
