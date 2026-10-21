// ReSharper disable All
#pragma once

#include "../../external/olc/olcPixelGameEngine.h"
#include "../Utils.h"
#include "Scene.h"
#include "Camera.h"
#include "TimeControl.h"
#include "DevTools/DevTools.h"

class GameManager;

class AcademiaEngine : public olc::PixelGameEngine
{
public:
    AcademiaEngine();

    bool OnUserCreate() override;
    bool OnUserUpdate(float elapsedTime) override;
    bool OnUserDestroy() override;

    olc::vi2d ConvertWorldPositionToPixels(const olc::vf2d& worldPosition) const;
    olc::vf2d ConvertPixelsToWorldPosition(const olc::vi2d& pixelsPosition) const;
    float ConvertWorldLengthToPixels(float worldLength) const;
    float ConvertPixelsToWorldLength(float pixelsLength) const;

    Scene& GetScene() { return _Scene; }
    Camera& GetCamera() { return _Camera; }
    TimeControl& GetTimeControl() { return _TimeControl; }
    DevTools& GetDevTools() { return _DevTools; }

    static AcademiaEngine* Instantiate();
    static AcademiaEngine* GetInstance();

protected:

    // Creates the game. Override to run something else instead (e.g. a visual test scenario).
    virtual GameManagerPtr CreateGameManager();

private:

    void PreUpdate(float elapsedTime);
    void Update(float elapsedTime);
    void PostUpdate(float elapsedTime);

    GameManagerPtr _GameManager = nullptr;
    Scene _Scene;
    Camera _Camera;
    TimeControl _TimeControl;
    DevTools _DevTools;
};
