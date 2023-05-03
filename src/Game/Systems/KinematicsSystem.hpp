#ifndef KinematicsSystem_INCLUDED
#define KinematicsSystem_INCLUDED

#include "system_begin_code.hpp"

class KinematicsSystem
	: public BaseSystem {
public:
	static Signature getSignature();

	void init();

	void quit();

	void update(Scalar dt);
};

#endif // KinematicsSystem_INCLUDED
