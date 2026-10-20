#pragma once
#include <Utils.h>

#define ACADEMIA_EXAMPLE

#ifdef ACADEMIA_EXAMPLE
#include "ExampleGameObject.h"
#endif

class AcademiaEngine;

class GameManager
{

public:

    void Initialize(AcademiaEngine* engineContext);
    void Update(float elapsedTime);
    void Uninitialize();

private:

    AcademiaEngine* _EngineContext = nullptr;

#ifdef ACADEMIA_EXAMPLE
    // The Scene owns the object, we only keep a reference to it.
    // Use _ExampleObject.lock() to access it: it returns nullptr once the object is destroyed.
    std::weak_ptr<ExampleGameObject> _ExampleObject;
#endif

};

