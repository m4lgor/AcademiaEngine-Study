#include "DevTools.h"

#include "../AcademiaEngine.h"

namespace
{
    constexpr olc::Key ToggleKey = olc::Key::F1;
    constexpr olc::Key PauseKey = olc::Key::F2;
    constexpr olc::Key StepKey = olc::Key::F3;
}

void DevTools::HandleInput(AcademiaEngine& engine)
{
    if (engine.GetKey(ToggleKey).bPressed)
        Visible = !Visible;

    // The keys work even while the tools are hidden
    TimeControl& timeControl = engine.GetTimeControl();
    if (engine.GetKey(PauseKey).bPressed)
        timeControl.TogglePause();

    if (engine.GetKey(StepKey).bPressed)
        timeControl.StepOneFrame();

    if (Visible)
        _PlayToolbar.HandleInput(engine);
}

void DevTools::Update(float elapsedTime)
{
    _StatsPanel.Update(elapsedTime);
}

void DevTools::Draw(AcademiaEngine& engine) const
{
    if (!Visible)
        return;

    _StatsPanel.Draw(engine);
    _PlayToolbar.Draw(engine);
}
