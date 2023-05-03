# Notes on using ECS

## Prerequisites  
You should be familiar with ECS model, specifically the ECS implementation design used by this game. See README.md (in the project's root) for the link to the guide.

Also note that ECS can be implemented in conjunction with any GUI or multimedia library, not just SDL2.

## Instructions  
ECS core code is located in the ``Entity``, ``Component``, ``System`` and ``ECS`` folders under the ``src/Core`` directory (hereafter the ``Core`` directory).  

ECS is capable of empowering any games that have the ECS mechanism/design nature. Here is how to make use of this ECS implementation:  
1. Create your component classes each of which in a separate header file, for example ``Renderable``, ``Transform`` and ``RigidBody``.  
2. Create your system classes each of which in a separate header file, for example ``RenderSystem``, ``KinematicsSystem`` and ``DynamicsSystem``.  
3. Create a main class for the game. Suppose it's the ``Game`` class. This class contains a variable of type ``ECS`` (declared in ``Core/ECS/ECS.hpp``) named ``ecs``.  
4. Write code to setup the game. For each component and system used in the game, you have to register it two times: register template instantiation (1) (see [Remarks](#Remarks)) and register via ``ecs`` (2).  
Note that registering (1) must be done with all classes used throughout the program, while registering (2) should be done with classes that are required by the contemporary ``ecs`` (i.e. at a specific time and for specific stage e.g. specific game levels, functions or screens).  

	```cpp
	// Game.cpp
	#include "AllComponents.hpp"
	#include "AllSystems.hpp"

	// REGISTER TEMPLATE INSTANTIATION (1) by including this header
	#include "Core/ECS/ecs_registry.hpp"
	// and use the macros to register components:
	USING_COMPONENT(Renderable);
	USING_COMPONENT(Transform);
	USING_COMPONENT(RigidBody);
	// and systems:
	USING_SYSTEM(RenderSystem);
	USING_SYSTEM(KinematicsSystem);
	USING_SYSTEM(DynamicsSystem);

	void Game::init() {
		ecs.init();
		// REGISTER VIA ECS VARIABLE (2): first, register the components:
		ecs.registerComponent<Renderable>();
		ecs.registerComponent<Transform>();
		ecs.registerComponent<RigidBody>();
		// then the systems:
		auto rend = ecs.registerSystem<RenderSystem>(s1);
		auto kin  = ecs.registerSystem<KinematicsSystem>(s2);
		auto dyn  = ecs.registerSystem<DynamicsSystem>(s3);
		// where s1, s2, s3 are the appropriate system's component signatures.

		// Now that the components and systems are all properly set up, we can
		// begin creating entities.
		EntityID playerID = ecs.createEntity();
		ecs.addComponentToEntity(playerID, Renderable(playerImage)); // just an example of how the Renderable component is supposed to be used
		ecs.addComponentToEntity(playerID, Transform(Position(0, 0));
		ecs.addComponentToEntity(playerID, RigidBody(Mass(15), Velocity(0, 0));
	}
	```
5. Write game loop. Every system should have an ``update()`` method to response to user events as well as update the entities' states after a specific amount of time.  
	```cpp
	// Game.cpp
	void Game::run() {
		while (running) {
			event = GetEvent(); // library-specific function
			rend->update(event);
			kin->update(event);
			dyn->update(event);
		}
	}
	```

## Remarks
The reason to have two separate phases of registering components and systems is that, while the second phase is obviously needed to establish component data arrays and maintain each system's entity list, the first phase is indeed introduced to solve the C++ linking problem when dealing with templates. The ``USING_COMPONENT`` and ``USING_SYSTEM`` macros actually expand to explicit template instantiation of the class type given to them as parameter, preventing such errors as "duplicate definitions" and "unresolved symbols" which would be likely to emerge if template instantiation is made implicitly by the compiler. For detailed explanation, please refer to [this answer on StackOverflow](https://stackoverflow.com/a/59614755/13680015).
