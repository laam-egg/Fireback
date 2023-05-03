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

#endif // ComponentManager_INCLUDED
