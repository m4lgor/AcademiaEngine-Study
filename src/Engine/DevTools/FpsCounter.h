#pragma once

class AcademiaEngine;

// Averages the frame time and draws a small FPS counter in the top-right corner
class FpsCounter
{
public:

    // Called every frame with the real (unpaused) frame time
    void Update(float elapsedTime);

    float GetFps() const { return _AverageFrameTime > 0.0f ? 1.0f / _AverageFrameTime : 0.0f; }
    float GetFrameTime() const { return _AverageFrameTime; }

    void Draw(AcademiaEngine& engine) const;

private:

    // Frame time is averaged over a short window so the numbers are readable
    float _AccumulatedTime = 0.0f;
    int _AccumulatedFrames = 0;
    float _AverageFrameTime = 0.0f;
};
