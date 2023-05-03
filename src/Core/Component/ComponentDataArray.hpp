#ifndef ComponentDataArray_INCLUDED
#define ComponentDataArray_INCLUDED

#include <array>
#include <unordered_map>
#include <functional>
#include "Core/Entity/EntityID.hpp"
#include "Core/Component/ComponentID.hpp"

class AbstractComponentDataArray {
public:
    virtual ~AbstractComponentDataArray() = default;

    virtual void onEntityDestroyed(EntityID id) = 0;

	virtual void restart() = 0;
};

/**
 * Class template for component arrays : each component type T
 * has its own ComponentDataArray<T> class.
 *
 * Each entity can possess certain components, each of which
 * essentially represents certain characteristics (1) and should be
 * specified by component-specific data (2). For example, an entity
 * possesses component type Transform, meaning that it can be moved
 * around and can be zoomed in and out (1) ; such traits should be
 * quantified by the entity's location x-y-z and the scale factor as
 * a positive float number (2).
 *
 * A component type has its own unique ComponentID and its specific data
 * is represented by a struct T. So, the purpose of a component array is
 * to store the component data of entities, meaning that any entity
 * possessing this component type will have its component data be an
 * element in this array.
 */
template<typename T>
class ComponentDataArray final
    : public AbstractComponentDataArray {
public:
	ComponentDataArray();

	virtual ~ComponentDataArray();

    void addComponentToEntity(EntityID entityID, T componentData);

    void removeComponentFromEntity(EntityID entityID);

    T& getComponentDataOfEntityAsRef(EntityID entityID);

    virtual void onEntityDestroyed(EntityID entityID) override;

	T& accessByIndex(size_t index, EntityID* entityIDPtr);

	size_t getSize() const;

	virtual void restart() override;

private:
	bool isComponentAddedToEntity(EntityID id);

	void ensureComponentAddedToEntity(EntityID id);

	std::array<T, MAX_ENTITIES> m_componentDataArray{};
	std::unordered_map<EntityID, size_t> m_entityID2ComponentArrayIndex{};
	std::unordered_map<size_t, EntityID> m_componentArrayIndex2EntityID{};
	size_t m_size{};
};

#endif // ComponentDataArray_INCLUDED
