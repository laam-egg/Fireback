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

#endif // ECS_INCLUDED
