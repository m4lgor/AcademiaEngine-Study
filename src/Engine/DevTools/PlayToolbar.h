#pragma once

#include "../../../external/olc/olcPixelGameEngine.h"
#include "../../Math/Vector.h"

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
        Vector2i Position;
        Vector2i Size;
        Vector2i PauseButton;
        Vector2i StepButton;
        Vector2i StatsButton;
    };

    static Layout GetLayout(const AcademiaEngine& engine);
};
