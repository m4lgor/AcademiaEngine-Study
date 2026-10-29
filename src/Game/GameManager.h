#pragma once
#include "../Engine/GameBase.h"

class ExampleGameObject;

class GameManager : public GameBase
{

public:

    void Initialize(AcademiaEngine* engineContext) override;
    void Update(Scene& scene, float elapsedTime) override;
    void FixedUpdate(Scene& scene, float fixedDeltaTime) override;
    void Uninitialize() override;

private:

    AcademiaEngine* _EngineContext = nullptr;

    // We added it, so we remove it (see Uninitialize)
    ExampleGameObject* _ExampleObject = nullptr;

};
