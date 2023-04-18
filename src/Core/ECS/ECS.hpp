#ifndef ECS_INCLUDED
#define ECS_INCLUDED

#include <memory>
#include "Core/Entity/EntityIDManager.hpp"
#include "Core/Component/ComponentManager.hpp"
#include "Core/System/SystemManager.hpp"

/**
 * This class acts as a mediator for integral interaction and
 * communication among Entities, Components and Systems.
 */
class ECS {
public:
	ECS();

	void init();

	template<typename T>
	ComponentID registerComponent();

	EntityID createEntity();

	template<typename T>
	void addComponentToEntity(EntityID entityID, T componentData);

	template<typename T>
	void removeComponentFromEntity(EntityID entityID);

	template<typename T>
	bool entityHasComponent(EntityID entityID);

	template<typename T>
	T& getComponentDataOfEntityAsRef(EntityID entityID);

	template<typename T>
	std::shared_ptr<ComponentDataArray<T>> getComponentDataArrayAsPtr() const;

	template<typename T>
	ComponentID getComponentID();

	void destroyEntity(EntityID entityID);

	void restart();

	template<typename T>
	std::shared_ptr<T> registerSystem(Signature sysSig);

private:
	std::unique_ptr<EntityIDManager> m_entityIDManagerPtr;
	std::unique_ptr<ComponentManager> m_componentManagerPtr;
	std::unique_ptr<SystemManager> m_systemManagerPtr;
};

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

#endif // ECS_INCLUDED
