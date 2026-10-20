#include "Scene.h"

#include <algorithm>

#include "AcademiaEngine.h"

void Scene::Destroy(const GameObjectPtr& object)
{
    if (object)
        object->_PendingDestroy = true;
}

void Scene::Clear()
{
    _ClearRequested = true;
}

void Scene::Update(float elapsedTime)
{
    for (const GameObjectPtr& object : _Objects)
    {
        if (!object->IsPendingDestroy())
            object->Update(elapsedTime);
    }
}

void Scene::Draw(AcademiaEngine& engine)
{
    for (const GameObjectPtr& object : _Objects)
    {
        if (!object->IsPendingDestroy())
            object->Draw(engine);
    }
}

void Scene::FlushPending()
{
    if (_ClearRequested)
    {
        _Objects.clear();
        _PendingSpawns.clear();
        _ClearRequested = false;
        return;
    }

    _Objects.erase(
        std::remove_if(_Objects.begin(), _Objects.end(),
            [](const GameObjectPtr& object) { return object->IsPendingDestroy(); }),
        _Objects.end());

    for (const GameObjectPtr& object : _PendingSpawns)
    {
        if (!object->IsPendingDestroy())
            _Objects.push_back(object);
    }
    _PendingSpawns.clear();
}
