#pragma once

#include "../../../external/olc/olcPixelGameEngine.h"

class AcademiaEngine;

// Buttons at the top center of the screen, like Unity's toolbar: pause, step, and stats panel
class PlayToolbar
{
public:

    // Reads mouse clicks on the buttons
    void HandleInput(AcademiaEngine& engine);

    void Draw(AcademiaEngine& engine) const;

private:

    struct Layout
    {
        olc::vi2d Position;
        olc::vi2d Size;
        olc::vi2d PauseButton;
        olc::vi2d StepButton;
        olc::vi2d StatsButton;
    };

    static Layout GetLayout(const AcademiaEngine& engine);
};
