#pragma once
#include <memory>

#include "../../external/olc/olcPixelGameEngine.h"

class AcademiaEngine;

class GameObject
{
public:
    virtual ~GameObject() = default;

    virtual void Update(float elapsedTime) {}
    virtual void Draw(AcademiaEngine& engine) {}

    bool IsPendingDestroy() const { return _PendingDestroy; }

    olc::vf2d Position;

private:
    friend class Scene;
    bool _PendingDestroy = false;
};

// True while the referenced object is alive and has not been destroyed.
// Returns false as soon as Destroy() is called, even though the object
// is only removed from the Scene at the end of the frame.
inline bool IsValid(const std::weak_ptr<GameObject>& object)
{
    const std::shared_ptr<GameObject> locked = object.lock();
    return locked != nullptr && !locked->IsPendingDestroy();
}
