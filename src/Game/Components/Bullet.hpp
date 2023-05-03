#ifndef Bullet_COMPONENT_INCLUDED
#define Bullet_COMPONENT_INCLUDED

#include "Game/BulletEmitter.hpp"

struct Bullet {
	BulletEmitter emitter{ EMITTER_UNKNOWN };

	Bullet() = default;

	Bullet(BulletEmitter _emitter)
		: emitter(_emitter) {
	}
};

#endif // Bullet_COMPONENT_INCLUDED
