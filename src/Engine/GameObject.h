#pragma once
#include "../../external/olc/olcPixelGameEngine.h"
#include "../Math/Vector.h"

class AcademiaEngine;

class GameObject
{
public:
    virtual ~GameObject() = default;

    // Once per frame, with the real frame time. Read input and decide what to do here.
    virtual void Update(float elapsedTime) {}

    // Zero, one or several times per frame, always with the same fixedDeltaTime.
    // Runs after Update and before the physics step: forces and velocities set here
    // are used by the simulation right after.
    virtual void FixedUpdate(float fixedDeltaTime) {}

    virtual void Draw(AcademiaEngine& engine) {}

    bool IsPendingRemove() const { return _PendingRemove; }

    Vector2f Position;

private:
    friend class Scene;
    bool _PendingRemove = false;
};
