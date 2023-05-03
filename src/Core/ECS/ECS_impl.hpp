#ifndef ECS_IMPL_INCLUDED
#define ECS_IMPL_INCLUDED

#include "Core/ECS/ECS.hpp"

//////////////////////////
// BEGIN IMPLEMENTATION //
//////////////////////////

void ECS::restart() {
	m_entityIDManagerPtr->restart();
	m_componentManagerPtr->restart();
	m_systemManagerPtr->restart();
}

ECS::ECS()
	: m_entityIDManagerPtr{},
	m_componentManagerPtr{},
	m_systemManagerPtr{} {
	// These pointers will be initialized by ECS::init()
	// since the managers require a lot of memory and thus
	// take considerable time to perform construction. Their
	// initialization process, therefore, should be issued
	// at scheduled time, especially when the user is explicitly
	// notified that a time-consuming task is taking place.
}

void ECS::init() {
	m_entityIDManagerPtr = std::make_unique<EntityIDManager>();
	m_componentManagerPtr = std::make_unique<ComponentManager>();
	m_systemManagerPtr = std::make_unique<SystemManager>();
	// restart();
}

template<typename T>
ComponentID ECS::registerComponent() {
	return m_componentManagerPtr->registerComponent<T>();
}

EntityID ECS::createEntity() {
	return m_entityIDManagerPtr->acquireEntityID();
}

template<typename T>
void ECS::addComponentToEntity(EntityID entityID, T componentData) {
	m_componentManagerPtr->addComponentToEntity<T>(entityID, componentData);

	Signature entitySig = m_entityIDManagerPtr->getEntitySignature(entityID);
	entitySig.enableComponent(m_componentManagerPtr->getComponentID<T>());
	m_entityIDManagerPtr->setEntitySignature(entityID, entitySig);

	m_systemManagerPtr->onEntitySignatureChanged(entityID, entitySig);
}

template<typename T>
void ECS::removeComponentFromEntity(EntityID entityID) {
	Signature entitySig = m_entityIDManagerPtr->getEntitySignature(entityID);
	entitySig.disableComponent(m_componentManagerPtr->getComponentID<T>());
	m_entityIDManagerPtr->setEntitySignature(entityID, entitySig);

	m_componentManagerPtr->removeComponentFromEntity<T>(entityID);

	m_systemManagerPtr->onEntitySignatureChanged(entityID, entitySig);
}

template<typename T>
bool ECS::entityHasComponent(EntityID entityID) {
	Signature entitySig = m_entityIDManagerPtr->getEntitySignature(entityID);
	ComponentID componentID = m_componentManagerPtr->getComponentID<T>();
	return entitySig.hasComponent(componentID);
}

template<typename T>
T& ECS::getComponentDataOfEntityAsRef(EntityID entityID) {
	return m_componentManagerPtr->getComponentDataOfEntityAsRef<T>(entityID);
}

template<typename T>
std::shared_ptr<ComponentDataArray<T>> ECS::getComponentDataArrayAsPtr() const {
	return m_componentManagerPtr->getComponentDataArrayAsPtr<T>();
}

template<typename T>
ComponentID ECS::getComponentID() {
	return m_componentManagerPtr->getComponentID<T>();
}

void ECS::destroyEntity(EntityID entityID) {
	m_entityIDManagerPtr->releaseEntityID(entityID);
	m_componentManagerPtr->onEntityDestroyed(entityID);
	m_systemManagerPtr->onEntityDestroyed(entityID);
}

template<typename T>
std::shared_ptr<T> ECS::registerSystem(Signature sysSig) {
	return m_systemManagerPtr->registerSystem<T>(sysSig);
}

///////////////////////////
// END OF IMPLEMENTATION //
///////////////////////////

#define ECS_INSTANTIATE_TEMPLATE_FOR_COMPONENT(T) \
	template ComponentID ECS::registerComponent<T>(); \
	template void ECS::addComponentToEntity<T>(EntityID entityID, T componentData); \
	template void ECS::removeComponentFromEntity<T>(EntityID entityID); \
	template bool ECS::entityHasComponent<T>(EntityID entityID); \
	template T& ECS::getComponentDataOfEntityAsRef<T>(EntityID entityID); \
	template std::shared_ptr<ComponentDataArray<T>> ECS::getComponentDataArrayAsPtr<T>() const; \
	template ComponentID ECS::getComponentID<T>()

#define ECS_INSTANTIATE_TEMPLATE_FOR_SYSTEM(T) \
	template std::shared_ptr<T> ECS::registerSystem<T>(Signature sysSig)

#endif // ECS_IMPL_INCLUDED
