#ifndef Bot_COMPONENT_INCLUDED
#define Bot_COMPONENT_INCLUDED

#include <memory>
#include "Game/BulletDistribution.hpp"

struct Bot {
	std::shared_ptr<BulletDistribution> bulletDistPtr;

	Bot()
		: bulletDistPtr(std::make_shared<BulletDistribution>()) {}
};

#endif // Bot_COMPONENT_INCLUDED
