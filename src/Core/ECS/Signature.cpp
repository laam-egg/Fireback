#include "Core/ECS/Signature.hpp"

/*
 * To avoid ambiguity, any call to a method of the base class std::bitset
 * will be prefixed with "this->". Methods that only belong to Signature
 * will be called via their bare name.
 */

void Signature::enableComponent(ComponentID id) {
	setComponentStatus(id, true);
}

void Signature::disableComponent(ComponentID id) {
	setComponentStatus(id, false);
}

void Signature::setComponentStatus(ComponentID id, bool enable) {
	this->set(id, enable);
}

bool Signature::hasComponent(ComponentID id) {
	return this->test(id);
}

bool Signature::match(Signature const& other) {
	return ((*this) & other) == other;
}