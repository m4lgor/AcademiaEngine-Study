#pragma once

class AcademiaEngine;
class FpsCounter;

// Top-left panel: FPS, object count, camera, mouse position and the debug keys
class StatsPanel
{
public:

    void Draw(AcademiaEngine& engine, const FpsCounter& fpsCounter) const;
};
