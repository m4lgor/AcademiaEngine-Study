#include "AcademiaEngine.h"

#include "../Game/GameManager.h"

static AcademiaEngine* _Instance = nullptr;

AcademiaEngine::AcademiaEngine()
{
    sAppName = "AcademiaEngine";
}

bool AcademiaEngine::OnUserCreate()
{
    _GameManager = std::make_shared<GameManager>();
    _GameManager->Initialize(this);

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
    if (_GameManager)
        _GameManager->Uninitialize();

    _Scene.Clear();
    _Scene.FlushPending();

    return true;
}

void AcademiaEngine::PreUpdate(float elapsedTime)
{
    Clear(olc::BLACK);
}

void AcademiaEngine::Update(float elapsedTime)
{
    if(_GameManager)
        _GameManager->Update(elapsedTime);

    _Scene.Update(elapsedTime);
    _Scene.Draw(*this);
}

void AcademiaEngine::PostUpdate(float elapsedTime)
{
    _Scene.FlushPending();
}

olc::vi2d AcademiaEngine::ConvertWorldPositionToPixels(const olc::vf2d& worldPosition) const
{
    return _Camera.WorldToScreen(worldPosition, GetScreenSize());
}

olc::vf2d AcademiaEngine::ConvertPixelsToWorldPosition(const olc::vi2d& pixelsPosition) const
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
