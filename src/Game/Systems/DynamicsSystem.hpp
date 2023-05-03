#ifndef DynamicsSystem_INCLUDED
#define DynamicsSystem_INCLUDED

#include "system_begin_code.hpp"

class DynamicsSystem
	: public BaseSystem {
public:
	static Signature getSignature();

	void init();

	void quit();

	void update(Scalar dt);
};

#endif // DynamicsSystem_INCLUDED
