# Fireback

## Introduction
This project is part of my learning Advanced Programming class (INT 2215 25) at UET-VNU.  
This is a game where a player try their best to avoid bullets emitted by a bot.

## Demo
You can grab a demo or feature-complete release of this game at <https://github.com/laam-egg/Fireback/releases/latest>. Just download the compressed file named ``Fireback-binary.zip`` under the ``Assets`` section, extract it to an arbitary folder and run the executable file ``Fireback.exe`` under the ``bin`` folder.

## How to play  
Coloring:  
 - RED means belonging to Bot.  
 - BLUE means belonging to Player.  
 - GREEN is the color of timer.  

Keybindings:  
 - Arrow keys **LEFT**, **RIGHT**, **UP**, **DOWN** to move around.  
 - **SPACE** to emit player's bullets (BLUE bullets), which could collide with bot's ones (RED bullets).  
 - **ENTER** to restart the game.

You, the player, aim to avoid bullets emitted by the bot as long as possible (to "endure" as long as you can). This is achieved by doing the following:  
1. Move around to avoid the RED bullets.  
2. Shoot to the bot (emit BLUE bullets) to prevent the RED bullets from reaching the player. (When collided, two bullets of any type will change their speed and direction based on physics. As a result, it is a powerful tool to defend yourself against the RED bullets).  

At first, you are given a number of bullets. The longer you endure, the more bullets you will be provided.

The game will finish when one RED bullet collides the player ; type **ENTER** for new game. (The BLUE bullets are friendly and have no effects on you).

## Implementation details
Though the project makes heavy use of modern C++ features including OOP, the dominant model of the game is ECS (Entity-Component-System) where:
 - each entity is referred to with a unique ID ;
 - components (which means "traits" or "features" of an entity) can be added to or removed from an entity ;
 - systems can operate on entities with certain components ;
 - and finally, a global mediator named ECS will take care of controlling the behavior of components and systems on entities.  

For example, an entity has components Transform and RigidBody will be managed by DynamicsSystem which takes care of its acceleration and collision forces among it and other entities. Any entity with Renderable component will be made available on the screen by RenderSystem.

I relied predominantly on [this guide to ECS](https://austinmorlan.com/posts/entity_component_system/), which provides excellent explanation and a concrete implementation approach. I have been improving the model so that it suits the needs of this project.

## TODO
 - Render entities as images.
 - Play sound.
 - Load background image (or not).
 - Arrange Components and Systems as separate files instead of placing all of them in main.cpp.
 - Add CMakeLists.txt.
 - Resolve TODO entries in all header and source files.

## Third-party libraries
 - [SDL2 and its plugins (SDL_image, SDL_ttf, SDL_mixer)](docs/README-SDL.txt)  

## Licensing
Copyright (C) 2023 Vu Tung Lam <vutunglaminfo@gmail.com>  
Distributed under the [3-clause BSD license](LICENSE.txt).
