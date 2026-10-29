#pragma once
#include <memory>

#include "Scene.h"

class AcademiaEngine;

// The base class of the game. The engine only knows this class, never the game itself,
// so the engine does not depend on the Game folder: the Game folder depends on the engine.
// main.cpp is where the two are connected.
//
// Update and FixedUpdate update the objects of the Scene. When you override them,
// call the base version (GameBase::Update, GameBase::FixedUpdate), or no object will update.
class GameBase
{
public:
    virtual ~GameBase() = default;

    virtual void Initialize(AcademiaEngine* engineContext) = 0;
    virtual void Uninitialize() = 0;

    // Once per frame, with the real frame time. Read input and decide what to do here.
    virtual void Update(Scene& scene, float elapsedTime) { scene.Update(elapsedTime); }

    // Zero, one or several times per frame, always with the same fixedDeltaTime.
    // Runs before the physics step: forces and velocities set here are used by the simulation right after.
    virtual void FixedUpdate(Scene& scene, float fixedDeltaTime) { scene.FixedUpdate(fixedDeltaTime); }
};

using GamePtr = std::unique_ptr<GameBase>;
