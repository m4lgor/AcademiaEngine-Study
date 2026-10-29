// ReSharper disable All
#pragma once

#include <utility>

#include "../../external/olc/olcPixelGameEngine.h"
#include "GameBase.h"
#include "Scene.h"
#include "Camera.h"
#include "TimeControl.h"
#include "FixedTimestep.h"
#include "DevTools/DevTools.h"
#include "../Math/Vector.h"

class AcademiaEngine : public olc::PixelGameEngine
{
public:
    AcademiaEngine();

    bool OnUserCreate() override;
    bool OnUserUpdate(float elapsedTime) override;
    bool OnUserDestroy() override;

    Vector2i ConvertWorldPositionToPixels(const Vector2f& worldPosition) const;
    Vector2f ConvertPixelsToWorldPosition(const Vector2i& pixelsPosition) const;
    float ConvertWorldLengthToPixels(float worldLength) const;
    float ConvertPixelsToWorldLength(float pixelsLength) const;

    Scene& GetScene() { return _Scene; }
    Camera& GetCamera() { return _Camera; }
    TimeControl& GetTimeControl() { return _TimeControl; }
    const FixedTimestep& GetFixedTimestep() const { return _FixedTimestep; }
    DevTools& GetDevTools() { return _DevTools; }

    // The game to run. Set it before Start(): the engine only knows the GameBase class.
    void SetGame(GamePtr game) { _Game = std::move(game); }

    static AcademiaEngine* Instantiate();
    static AcademiaEngine* GetInstance();

private:

    void PreUpdate(float elapsedTime);
    void Update(float elapsedTime);
    void FixedUpdate(float fixedDeltaTime);
    void DrawScene();
    void PostUpdate(float elapsedTime);

    GamePtr _Game = nullptr;
    Scene _Scene;
    Camera _Camera;
    TimeControl _TimeControl;
    FixedTimestep _FixedTimestep;
    DevTools _DevTools;
};
