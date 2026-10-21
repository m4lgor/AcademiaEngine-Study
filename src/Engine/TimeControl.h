#pragma once

// Game time: pause and single step.
// Every frame, the engine asks it whether gameplay should update and with which elapsed time.
// Drawing is not affected: a paused game is still drawn.
class TimeControl
{
public:

    // Called by the engine at the start of every frame with the real frame time.
    // Returns false when gameplay should not update this frame.
    bool Tick(float realElapsedTime);

    // Elapsed time given to gameplay this frame
    float GetElapsedTime() const { return _ElapsedTime; }

    bool IsPaused() const { return _Paused; }
    void SetPaused(bool paused) { _Paused = paused; }
    void TogglePause() { _Paused = !_Paused; }

    // While paused, advances the game by one step of StepDeltaTime on the next frame
    void StepOneFrame();

    static constexpr float StepDeltaTime = 1.0f / 60.0f;

private:

    bool _Paused = false;
    bool _StepRequested = false;
    float _ElapsedTime = 0.0f;
};
