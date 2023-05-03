#ifndef BotSystem_INCLUDED
#define BotSystem_INCLUDED

#include "system_begin_code.hpp"

class BotSystem
	: public BaseSystem {
public:
	static Signature getSignature();

	void init();

	void quit();

	void update(Scalar dt);
};

#endif // BotSystem_INCLUDED
