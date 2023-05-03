#ifndef SystemManager_INCLUDED
#define SystemManager_INCLUDED

#include <unordered_map>
#include <memory>
#include "Core/Exception.hpp"
#include "Core/ECS/Signature.hpp"
#include "Core/Entity/EntityID.hpp"
#include "Core/System/BaseSystem.hpp"

using SystemLookupKey = char const*;

class SystemManager {
public:
	SystemManager();

	template<typename T>
	std::shared_ptr<T> registerSystem(Signature systemSig);

	/**
	 * This function gets called when a component is added to or removed
	 * from an entity.
	 */
	void onEntitySignatureChanged(EntityID entityID, Signature newEntitySig);

	void onEntityDestroyed(EntityID entityID);

	void restart();

private:
	std::unordered_map<SystemLookupKey, std::shared_ptr<BaseSystem>> m_systems{};
	std::unordered_map<SystemLookupKey, Signature> m_systemSignatures{};

	template<typename T>
	static SystemLookupKey getLookupKeyOfAnySystem();
};

#endif // SystemManager_INCLUDED
