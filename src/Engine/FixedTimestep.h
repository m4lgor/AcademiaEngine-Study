#pragma once

// Turns the real frame time into fixed steps of constant duration.
// A frame can need zero, one or several steps, but every step lasts exactly DeltaTime.
// This is what makes FixedUpdate (and later the physics) behave the same on every computer.
class FixedTimestep
{
public:

    // Duration of every fixed step: 60 steps per second
    static constexpr float DeltaTime = 1.0f / 60.0f;

    // A very long frame (a breakpoint, a dragged window) is cut to this, so the game
    // does not try to catch up with hundreds of steps in one frame
    static constexpr float MaxFrameTime = 0.25f;

    // Adds the time of this frame and returns how many fixed steps to run now
    int Advance(float elapsedTime);

    // How far we are between the last step and the next one, from 0 to 1.
    // Useful to draw smoothly between two steps.
    float GetAlpha() const { return _Accumulator / DeltaTime; }

private:

    float _Accumulator = 0.0f;
};
