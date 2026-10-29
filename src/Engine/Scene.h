#pragma once
#include <vector>
#include <memory>
#include <utility>
#include <type_traits>

#include "GameObject.h"

class AcademiaEngine;

// Holds every GameObject in the game.
//
// Whoever calls Add is responsible for calling Remove, like new and delete.
// The pointer returned by Add is only a view: keep it while you need the object,
// and set it to nullptr once you have removed it.
//
// Add and Remove take effect at the end of the frame, so they are safe to call
// from inside a GameObject's Update.
class Scene
{
public:

    Scene() = default;
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    // Reports objects that were added but never removed
    ~Scene();

    template<typename T, typename... Args>
    T* Add(Args&&... args)
    {
        static_assert(std::is_base_of_v<GameObject, T>, "Scene::Add<T>: T must derive from GameObject");

        std::unique_ptr<T> object = std::make_unique<T>(std::forward<Args>(args)...);
        T* pointer = object.get();
        _PendingAdds.push_back(std::move(object));
        return pointer;
    }

    // The object is deleted at the end of the frame: do not use the pointer after this call.
    void Remove(GameObject* object);

    // Removes every object, for example to start a new level
    void Clear();

    void Update(float elapsedTime);
    void FixedUpdate(float fixedDeltaTime);
    void Draw(AcademiaEngine& engine);

    // Applies pending additions and removals. Called by the engine at the end of the frame.
    void FlushPending();

    // Prints every object that is still in the Scene and deletes it. Returns how many there were.
    // Called by the engine at shutdown, once the game has removed what it added.
    size_t ReportLeaks();

    size_t GetObjectCount() const { return _Objects.size(); }

private:

    std::vector<std::unique_ptr<GameObject>> _Objects;
    std::vector<std::unique_ptr<GameObject>> _PendingAdds;
    bool _ClearRequested = false;
};
