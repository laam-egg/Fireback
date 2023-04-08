#ifndef Signature_INCLUDED
#define Signature_INCLUDED

#include <bitset>
#include "Core/Component/ComponentID.hpp"

/**
 * A Signature is essentially an std::bitset indicating
 * which components are enabled (and disabled) based on
 * the components' IDs. A Signature instance is used by
 * an EntityManager or SystemManager to save components
 * that it is concerned with.
 */
class Signature
	: public std::bitset<MAX_COMPONENT_ID> {
public:
	using std::bitset<MAX_COMPONENT_ID>::bitset;

	// TODO: template<typename T> void enableComponent();
	// => usage: signature.enableComponent<Transform>();
	// => global access to ECS.
	void enableComponent(ComponentID id);
	void disableComponent(ComponentID id);
	void setComponentStatus(ComponentID id, bool enable);
	bool hasComponent(ComponentID id);

	/**
	 * Returns true if ((*this & other) == other) and false
	 * otherwise.
	 */
	bool match(Signature const& other);
};

#endif // Signature_INCLUDED
