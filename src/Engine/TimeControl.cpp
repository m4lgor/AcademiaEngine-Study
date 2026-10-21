#include "TimeControl.h"

bool TimeControl::Tick(float realElapsedTime)
{
    const bool stepRequested = _StepRequested;
    _StepRequested = false;

    if (!_Paused)
    {
        _ElapsedTime = realElapsedTime;
        return true;
    }

    // Paused: skip gameplay, unless a single step was requested
    _ElapsedTime = stepRequested ? StepDeltaTime : 0.0f;
    return stepRequested;
}

void TimeControl::StepOneFrame()
{
    if (_Paused)
        _StepRequested = true;
}
