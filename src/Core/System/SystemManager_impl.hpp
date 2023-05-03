#ifndef SystemManager_IMPL_INCLUDED
#define SystemManager_IMPL_INCLUDED

#include "Core/System/SystemManager.hpp"

//////////////////////////
// BEGIN IMPLEMENTATION //
//////////////////////////

void SystemManager::restart() {
	for (auto const& pair : m_systems) {
		auto const& systemPtr = pair.second;
		systemPtr->entityIDs.clear();
	}
}

SystemManager::SystemManager() {
	// restart();
}

template<typename T>
std::shared_ptr<T> SystemManager::registerSystem(Signature systemSig) {
	SystemLookupKey k = getLookupKeyOfAnySystem<T>();
	if (m_systems.find(k) != m_systems.end()) {
		throw Exception("System already registered.");
	}
	std::shared_ptr<T> sys = std::make_shared<T>();
	m_systems[k] = sys;
	m_systemSignatures[k] = systemSig;
	return sys;
}

void SystemManager::onEntitySignatureChanged(EntityID entityID, Signature newEntitySig) {
	for (auto& pair : m_systems) {
		auto& k = pair.first;
		auto& sysPtr = pair.second;
		Signature sysSig = m_systemSignatures[k];
		if (newEntitySig.match(sysSig)) {
			sysPtr->entityIDs.insert(entityID);
		}
		else {
			sysPtr->entityIDs.erase(entityID);
		}
	}
}

void SystemManager::onEntityDestroyed(EntityID entityID) {
	for (auto& pair : m_systems) {
		auto& sysPtr = pair.second;
		// No need to check whether entityID is available in the set
		// since std::set.erase automatically handle such case.
		sysPtr->entityIDs.erase(entityID);
	}
}

template<typename T>
SystemLookupKey SystemManager::getLookupKeyOfAnySystem() {
	return typeid(T).name();
}

///////////////////////////
// END OF IMPLEMENTATION //
///////////////////////////

#define SystemManager_INSTANTIATE_TEMPLATE_FOR_SYSTEM(T) \
	template std::shared_ptr<T> SystemManager::registerSystem<T>(Signature systemSig); \
	template SystemLookupKey SystemManager::getLookupKeyOfAnySystem<T>()

#endif // SystemManager_IMPL_INCLUDED
