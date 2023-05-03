#ifndef PlayerSystem_INCLUDED
#define PlayerSystem_INCLUDED

#include "system_begin_code.hpp"
#include <memory>
#include "Core/Component/ComponentDataArray.hpp"

class PlayerSystem
	: public BaseSystem {
public:
	static Signature getSignature();

	PlayerSystem();

	void init();

	void quit();

	void update(Scalar dt);

private:
	Uint8 const* m_k;
	Vector findPositionOfNearestBot(Vector const& playerPosition, std::shared_ptr<ComponentDataArray<Bot>> const& botArrayPtr);
};

#endif // PlayerSystem_INCLUDED
