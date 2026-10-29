#include "GameManager.h"
#include "../../external/olc/olcPixelGameEngine.h"
#include "../Engine/AcademiaEngine.h"
#include "ExampleGameObject.h"

void GameManager::Initialize(AcademiaEngine* engineContext)
{
    _EngineContext = engineContext;

    _ExampleObject = _EngineContext->GetScene().Add<ExampleGameObject>();
    _ExampleObject->Position = { 0.0f, 0.0f };
}

void GameManager::Update(Scene& scene, float elapsedTime)
{
    constexpr olc::Key rightKey = olc::Key::D;
    constexpr olc::Key leftKey = olc::Key::A;
    constexpr olc::Key upKey = olc::Key::W;
    constexpr olc::Key downKey = olc::Key::S;
    constexpr olc::Key spaceKey = olc::Key::SPACE;
    const olc::HWButton moveRightButton = _EngineContext->GetKey(rightKey);
    const olc::HWButton moveLeftButton = _EngineContext->GetKey(leftKey);
    const olc::HWButton moveUpButton = _EngineContext->GetKey(upKey);
    const olc::HWButton moveDownButton = _EngineContext->GetKey(downKey);
    const olc::HWButton jumpButton = _EngineContext->GetKey(spaceKey);
    
    // Gameplay code

    // Updates every GameObject of the Scene. Without this call, no object updates.
    // Our code above runs first, then the objects.
    GameBase::Update(scene, elapsedTime);
}

void GameManager::FixedUpdate(Scene& scene, float fixedDeltaTime)
{
    // Runs before the physics step, with the same fixedDeltaTime every time.
    // Apply forces and velocities here. Read input in Update, not here: a key press
    // lasts one frame, and a frame can run zero or several fixed steps.

    // Calls FixedUpdate on every GameObject of the Scene
    GameBase::FixedUpdate(scene, fixedDeltaTime);
}

void GameManager::Uninitialize()
{
    _EngineContext->GetScene().Remove(_ExampleObject);
    _ExampleObject = nullptr;
}
