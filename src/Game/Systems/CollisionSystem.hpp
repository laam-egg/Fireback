#ifndef CollisionSystem_INCLUDED
#define CollisionSystem_INCLUDED

#include "system_begin_code.hpp"

class CollisionSystem
	: public BaseSystem {
public:
	static Signature getSignature();

	void init();

	void quit();

	CollisionSystem();

	~CollisionSystem();

	void update(Scalar dt);

private:
	RigidBody const* m_rbMock{ nullptr };
};

#endif // CollisionSystem_INCLUDED
