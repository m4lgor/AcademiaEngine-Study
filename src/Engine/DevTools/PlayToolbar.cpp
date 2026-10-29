#include "PlayToolbar.h"

#include "../AcademiaEngine.h"
#include "DevToolsStyle.h"

using namespace DevToolsStyle;

namespace
{
    constexpr int ButtonSize = 34;
    constexpr int ButtonGap = 4;
    constexpr int GroupGap = 14;         // Between the time buttons and the stats button
    constexpr int ToolbarPadding = 4;

    bool IsInButton(const Vector2i& button, const Vector2i& point)
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
    layout.Size = { ButtonSize * 3 + ButtonGap + GroupGap + ToolbarPadding * 2, ButtonSize + ToolbarPadding * 2 };
    layout.Position = { (engine.ScreenWidth() - layout.Size.x) / 2, ScreenMargin };
    layout.PauseButton = layout.Position + Vector2i{ ToolbarPadding, ToolbarPadding };
    layout.StepButton = layout.PauseButton + Vector2i{ ButtonSize + ButtonGap, 0 };
    layout.StatsButton = layout.StepButton + Vector2i{ ButtonSize + GroupGap, 0 };
    return layout;
}

void PlayToolbar::HandleInput(AcademiaEngine& engine)
{
    if (!engine.GetMouse(0).bPressed)
        return;

    const Layout layout = GetLayout(engine);
    const Vector2i mouse = engine.GetMousePos();
    TimeControl& timeControl = engine.GetTimeControl();

    if (IsInButton(layout.PauseButton, mouse))
        timeControl.TogglePause();
    else if (IsInButton(layout.StepButton, mouse))
        timeControl.StepOneFrame();
    else if (IsInButton(layout.StatsButton, mouse))
        engine.GetDevTools().ShowStats = !engine.GetDevTools().ShowStats;
}

void PlayToolbar::Draw(AcademiaEngine& engine) const
{
    const Layout layout = GetLayout(engine);
    const Vector2i mouse = engine.GetMousePos();
    const bool paused = engine.GetTimeControl().IsPaused();

    DrawPanel(engine, layout.Position, layout.Size);

    // Pause: two bars, highlighted while the game is paused
    {
        const Vector2i button = layout.PauseButton;
        FillPanel(engine, button, { ButtonSize, ButtonSize }, ButtonColor(paused, IsInButton(button, mouse)));
        const Vector2i center = button + Vector2i{ ButtonSize / 2, ButtonSize / 2 };
        engine.FillRect(center + Vector2i{ -7, -8 }, { 5, 16 }, ValueColor);
        engine.FillRect(center + Vector2i{ 2, -8 }, { 5, 16 }, ValueColor);
    }

    // Step: triangle and bar, only usable while paused
    {
        const Vector2i button = layout.StepButton;
        const bool hovered = paused && IsInButton(button, mouse);
        FillPanel(engine, button, { ButtonSize, ButtonSize }, ButtonColor(false, hovered));
        const Vector2i center = button + Vector2i{ ButtonSize / 2, ButtonSize / 2 };
        const olc::Pixel iconColor = paused ? ValueColor : DisabledColor;
        engine.FillTriangle(center + Vector2i{ -7, -8 }, center + Vector2i{ -7, 8 }, center + Vector2i{ 3, 0 }, iconColor);
        engine.FillRect(center + Vector2i{ 4, -8 }, { 4, 17 }, iconColor);
    }

    // Stats: bar chart, highlighted while the stats panel is open
    {
        const Vector2i button = layout.StatsButton;
        FillPanel(engine, button, { ButtonSize, ButtonSize }, ButtonColor(engine.GetDevTools().ShowStats, IsInButton(button, mouse)));
        const Vector2i bottomLeft = button + Vector2i{ ButtonSize / 2 - 9, ButtonSize / 2 + 8 };
        engine.FillRect(bottomLeft + Vector2i{ 0, -7 }, { 4, 7 }, ValueColor);
        engine.FillRect(bottomLeft + Vector2i{ 7, -16 }, { 4, 16 }, ValueColor);
        engine.FillRect(bottomLeft + Vector2i{ 14, -11 }, { 4, 11 }, ValueColor);
    }
}
