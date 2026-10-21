#pragma once

class AcademiaEngine;

// Top-left panel: FPS, object count, camera, mouse position and the debug keys
class StatsPanel
{
public:

    // Called every frame with the real (unpaused) frame time
    void Update(float elapsedTime);

    void Draw(AcademiaEngine& engine) const;

private:

    // Frame time is averaged over a short window so the numbers are readable
    float _AccumulatedTime = 0.0f;
    int _AccumulatedFrames = 0;
    float _AverageFrameTime = 0.0f;
};
