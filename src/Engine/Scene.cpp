#include "Scene.h"

#include <algorithm>
#include <cstdio>
#include <typeinfo>

#include "AcademiaEngine.h"

Scene::~Scene()
{
    ReportLeaks();
}

void Scene::Remove(GameObject* object)
{
    if (object)
        object->_PendingRemove = true;
}

void Scene::Clear()
{
    _ClearRequested = true;
}

void Scene::Update(float elapsedTime)
{
    for (const std::unique_ptr<GameObject>& object : _Objects)
    {
        if (!object->IsPendingRemove())
            object->Update(elapsedTime);
    }
}

void Scene::FixedUpdate(float fixedDeltaTime)
{
    for (const std::unique_ptr<GameObject>& object : _Objects)
    {
        if (!object->IsPendingRemove())
            object->FixedUpdate(fixedDeltaTime);
    }
}

void Scene::Draw(AcademiaEngine& engine)
{
    for (const std::unique_ptr<GameObject>& object : _Objects)
    {
        if (!object->IsPendingRemove())
            object->Draw(engine);
    }
}

void Scene::FlushPending()
{
    if (_ClearRequested)
    {
        _Objects.clear();
        _PendingAdds.clear();
        _ClearRequested = false;
        return;
    }

    _Objects.erase(
        std::remove_if(_Objects.begin(), _Objects.end(),
            [](const std::unique_ptr<GameObject>& object) { return object->IsPendingRemove(); }),
        _Objects.end());

    for (std::unique_ptr<GameObject>& object : _PendingAdds)
    {
        if (!object->IsPendingRemove())
            _Objects.push_back(std::move(object));
    }
    _PendingAdds.clear();
}

size_t Scene::ReportLeaks()
{
    FlushPending();

    const size_t leakCount = _Objects.size();
    if (leakCount > 0)
    {
        std::fprintf(stderr, "Scene: %zu object(s) were added but never removed. "
            "Whoever calls Scene::Add must call Scene::Remove.\n", leakCount);
        for (const std::unique_ptr<GameObject>& object : _Objects)
            std::fprintf(stderr, "  - %s\n", typeid(*object).name());
    }

    _Objects.clear();
    return leakCount;
}
