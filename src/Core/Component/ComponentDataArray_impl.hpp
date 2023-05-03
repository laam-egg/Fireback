#ifndef ComponentDataArray_IMPL_INCLUDED
#define ComponentDataArray_IMPL_INCLUDED

#include "Core/Component/ComponentDataArray.hpp"

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

#define ComponentDataArray_INSTANTIATE_TEMPLATE_FOR_COMPONENT(T) \
	template class ComponentDataArray<T>

#endif // ComponentDataArray_IMPL_INCLUDED
