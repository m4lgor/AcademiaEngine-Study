#pragma once
#include <memory>
#include <vector>
#include <utility>
#include <type_traits>

#include "../Game/GameObject.h"

class AcademiaEngine;

using GameObjectPtr = std::shared_ptr<GameObject>;

// Owns every GameObject in the game.
// Spawn and Destroy are deferred to the end of the frame, so they are safe to call
// from inside a GameObject's Update (e.g. a bullet destroying itself on hit).
class Scene
{
public:

    template<typename T, typename... Args>
    std::shared_ptr<T> Spawn(Args&&... args)
    {
        static_assert(std::is_base_of_v<GameObject, T>, "Spawn<T>: T must derive from GameObject");

        std::shared_ptr<T> object = std::make_shared<T>(std::forward<Args>(args)...);
        _PendingSpawns.push_back(object);
        return object;
    }

    void Destroy(const GameObjectPtr& object);
    void Clear();

    void Update(float elapsedTime);
    void Draw(AcademiaEngine& engine);

    // Applies pending spawns and destroys. Called by the engine at the end of the frame.
    void FlushPending();

    const std::vector<GameObjectPtr>& GetObjects() const { return _Objects; }
    size_t GetObjectCount() const { return _Objects.size(); }

private:

    std::vector<GameObjectPtr> _Objects;
    std::vector<GameObjectPtr> _PendingSpawns;
    bool _ClearRequested = false;
};
