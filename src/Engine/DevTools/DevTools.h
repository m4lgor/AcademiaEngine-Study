#pragma once

#include "FpsCounter.h"
#include "StatsPanel.h"
#include "PlayToolbar.h"

class AcademiaEngine;

// In-game developer tools: the panels drawn on top of the game and the debug keys.
// The tools only display and drive engine systems (TimeControl, Camera...), they hold no game state.
//   F1  show / hide the dev tools (the FPS counter stays)
//   F2  pause / resume the game
//   F3  while paused, advance the game by one step
class DevTools
{
public:

    // Reads the debug keys and the panels' mouse input. Called by the engine at the start of every frame.
    void HandleInput(AcademiaEngine& engine);

    // Called by the engine every frame with the real (unpaused) frame time
    void Update(float elapsedTime);

    // Called by the engine after the Scene is drawn, so the tools stay on top
    void Draw(AcademiaEngine& engine) const;

    bool Visible = true;

    // The stats panel is opened with the toolbar's stats button
    bool ShowStats = false;

private:

    FpsCounter _FpsCounter;
    StatsPanel _StatsPanel;
    PlayToolbar _PlayToolbar;
};
