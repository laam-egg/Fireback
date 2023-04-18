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


//////////////////////////
// BEGIN IMPLEMENTATION //
//////////////////////////

#include "Core/Exception.hpp"

template<typename T>
void ComponentDataArray<T>::restart() {
	T zeroComponentData{};
	m_componentDataArray.fill(zeroComponentData);
	m_entityID2ComponentArrayIndex.clear();
	m_componentArrayIndex2EntityID.clear();
	m_size = 0;
}

template<typename T>
ComponentDataArray<T>::ComponentDataArray() {
	// restart();
}

template<typename T>
ComponentDataArray<T>::~ComponentDataArray() {
	// Nothing
}

template<typename T>
void ComponentDataArray<T>::addComponentToEntity(EntityID entityID, T componentData) {
	if (isComponentAddedToEntity(entityID)) {
		throw Exception("Component already added to entity.");
	}
	auto const newIndex = m_size;
	m_componentDataArray[newIndex] = componentData;
	m_entityID2ComponentArrayIndex[entityID] = newIndex;
	m_componentArrayIndex2EntityID[newIndex] = entityID;

	++m_size;
}

template<typename T>
void ComponentDataArray<T>::removeComponentFromEntity(EntityID id) {
	ensureComponentAddedToEntity(id);
	auto const removedIndex = m_entityID2ComponentArrayIndex[id];
	auto const lastIndex = m_size - 1;
	auto const IDOfLastEntity = m_componentArrayIndex2EntityID[lastIndex];

	m_componentDataArray[removedIndex] = m_componentDataArray[lastIndex];
	m_entityID2ComponentArrayIndex[IDOfLastEntity] = removedIndex;
	m_componentArrayIndex2EntityID[removedIndex] = IDOfLastEntity;

	m_entityID2ComponentArrayIndex.erase(id);
	m_componentArrayIndex2EntityID.erase(lastIndex);

	--m_size;
}

template<typename T>
T& ComponentDataArray<T>::getComponentDataOfEntityAsRef(EntityID id) {
	ensureComponentAddedToEntity(id);
	return m_componentDataArray[m_entityID2ComponentArrayIndex[id]];
}

template<typename T>
void ComponentDataArray<T>::onEntityDestroyed(EntityID id) {
	if (isComponentAddedToEntity(id)) {
		removeComponentFromEntity(id);
	}
}

template<typename T>
T& ComponentDataArray<T>::accessByIndex(size_t index, EntityID* entityIDPtr) {
	if (index >= m_size) {
		throw Exception("Array index out of range.");
	}
	if (entityIDPtr != nullptr) {
		*entityIDPtr = m_componentArrayIndex2EntityID[index];
	}
	return m_componentDataArray[index];
}

template<typename T>
size_t ComponentDataArray<T>::getSize() const {
	return m_size;
}

template<typename T>
inline bool ComponentDataArray<T>::isComponentAddedToEntity(EntityID id) {
	return m_entityID2ComponentArrayIndex.find(id) != m_entityID2ComponentArrayIndex.end();
}

template<typename T>
inline void ComponentDataArray<T>::ensureComponentAddedToEntity(EntityID id) {
	if (!isComponentAddedToEntity(id)) {
		throw Exception("Component not yet added to entity.");
	}
}
///////////////////////////
// END OF IMPLEMENTATION //
///////////////////////////

#endif // ComponentDataArray_INCLUDED
