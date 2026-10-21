#include "PlayToolbar.h"

#include "../AcademiaEngine.h"
#include "DevToolsStyle.h"

using namespace DevToolsStyle;

namespace
{
    constexpr int ButtonSize = 34;
    constexpr int ButtonGap = 4;
    constexpr int ToolbarPadding = 4;

    bool IsInButton(const olc::vi2d& button, const olc::vi2d& point)
    {
        return point.x >= button.x && point.x < button.x + ButtonSize
            && point.y >= button.y && point.y < button.y + ButtonSize;
    }

    olc::Pixel ButtonColor(bool active, bool hovered)
    {
        if (active)
            return ActiveColor;
        return hovered ? HoverColor : KeyColor;
    }
}

PlayToolbar::Layout PlayToolbar::GetLayout(const AcademiaEngine& engine)
{
    Layout layout;
    layout.Size = { ButtonSize * 2 + ButtonGap + ToolbarPadding * 2, ButtonSize + ToolbarPadding * 2 };
    layout.Position = { (engine.ScreenWidth() - layout.Size.x) / 2, ScreenMargin };
    layout.PauseButton = layout.Position + olc::vi2d{ ToolbarPadding, ToolbarPadding };
    layout.StepButton = layout.PauseButton + olc::vi2d{ ButtonSize + ButtonGap, 0 };
    return layout;
}

void PlayToolbar::HandleInput(AcademiaEngine& engine)
{
    if (!engine.GetMouse(0).bPressed)
        return;

    const Layout layout = GetLayout(engine);
    const olc::vi2d mouse = engine.GetMousePos();
    TimeControl& timeControl = engine.GetTimeControl();

    if (IsInButton(layout.PauseButton, mouse))
        timeControl.TogglePause();
    else if (IsInButton(layout.StepButton, mouse))
        timeControl.StepOneFrame();
}

void PlayToolbar::Draw(AcademiaEngine& engine) const
{
    const Layout layout = GetLayout(engine);
    const olc::vi2d mouse = engine.GetMousePos();
    const bool paused = engine.GetTimeControl().IsPaused();

    DrawPanel(engine, layout.Position, layout.Size);

    // Pause: two bars, highlighted while the game is paused
    {
        const olc::vi2d button = layout.PauseButton;
        FillPanel(engine, button, { ButtonSize, ButtonSize }, ButtonColor(paused, IsInButton(button, mouse)));
        const olc::vi2d center = button + olc::vi2d{ ButtonSize / 2, ButtonSize / 2 };
        engine.FillRect(center + olc::vi2d{ -7, -8 }, { 5, 16 }, ValueColor);
        engine.FillRect(center + olc::vi2d{ 2, -8 }, { 5, 16 }, ValueColor);
    }

    // Step: triangle and bar, only usable while paused
    {
        const olc::vi2d button = layout.StepButton;
        const bool hovered = paused && IsInButton(button, mouse);
        FillPanel(engine, button, { ButtonSize, ButtonSize }, ButtonColor(false, hovered));
        const olc::vi2d center = button + olc::vi2d{ ButtonSize / 2, ButtonSize / 2 };
        const olc::Pixel iconColor = paused ? ValueColor : DisabledColor;
        engine.FillTriangle(center + olc::vi2d{ -7, -8 }, center + olc::vi2d{ -7, 8 }, center + olc::vi2d{ 3, 0 }, iconColor);
        engine.FillRect(center + olc::vi2d{ 4, -8 }, { 4, 17 }, iconColor);
    }
}
