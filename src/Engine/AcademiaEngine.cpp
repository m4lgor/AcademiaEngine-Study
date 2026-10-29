#include "AcademiaEngine.h"

static AcademiaEngine* _Instance = nullptr;

AcademiaEngine::AcademiaEngine()
{
    sAppName = "AcademiaEngine";
}

bool AcademiaEngine::OnUserCreate()
{
    if (_Game)
        _Game->Initialize(this);

    // Objects spawned during Initialize are live on the first frame
    _Scene.FlushPending();

    return true;
}

bool AcademiaEngine::OnUserUpdate(float elapsedTime)
{
    PreUpdate(elapsedTime);
    Update(elapsedTime);
    PostUpdate(elapsedTime);

    return true;
}

bool AcademiaEngine::OnUserDestroy()
{
    if (_Game)
        _Game->Uninitialize();

    // The game must have removed everything it added by now
    _Scene.ReportLeaks();

    return true;
}

void AcademiaEngine::PreUpdate(float elapsedTime)
{
    _DevTools.HandleInput(*this);
    _DevTools.Update(elapsedTime);

    Clear(olc::BLACK);
}

void AcademiaEngine::Update(float elapsedTime)
{
    // Paused: skip gameplay, unless a single step was requested
    if (!_TimeControl.Tick(elapsedTime))
        return;

    const float gameplayTime = _TimeControl.GetElapsedTime();

    // Once per frame. The game updates the Scene (GameBase::Update does it by default).
    if (_Game)
        _Game->Update(_Scene, gameplayTime);

    // Zero, one or several fixed steps, all of the same duration
    const int stepCount = _FixedTimestep.Advance(gameplayTime);
    for (int step = 0; step < stepCount; ++step)
        FixedUpdate(FixedTimestep::DeltaTime);
}

void AcademiaEngine::FixedUpdate(float fixedDeltaTime)
{
    // Gameplay goes first, so what it sets (forces, velocities) is used by the physics step that follows
    if (_Game)
        _Game->FixedUpdate(_Scene, fixedDeltaTime);
}

void AcademiaEngine::PostUpdate(float elapsedTime)
{
    _Scene.FlushPending();

    // After the simulation and the flush, so we draw what exists at the end of this frame.
    // Also runs while paused.
    _Scene.Draw(*this);

    _DevTools.Draw(*this);
}

Vector2i AcademiaEngine::ConvertWorldPositionToPixels(const Vector2f& worldPosition) const
{
    return _Camera.WorldToScreen(worldPosition, GetScreenSize());
}

Vector2f AcademiaEngine::ConvertPixelsToWorldPosition(const Vector2i& pixelsPosition) const
{
    return _Camera.ScreenToWorld(pixelsPosition, GetScreenSize());
}

float AcademiaEngine::ConvertWorldLengthToPixels(float worldLength) const
{
    return _Camera.WorldToScreenLength(worldLength);
}

float AcademiaEngine::ConvertPixelsToWorldLength(float pixelsLength) const
{
    return _Camera.ScreenToWorldLength(pixelsLength);
}

AcademiaEngine* AcademiaEngine::Instantiate()
{
    if (!_Instance)
    {
        _Instance = new AcademiaEngine;
    }

    return _Instance;
}

AcademiaEngine* AcademiaEngine::GetInstance()
{
    return _Instance;
}
