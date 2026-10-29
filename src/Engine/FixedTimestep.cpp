#include "FixedTimestep.h"

#include <algorithm>

int FixedTimestep::Advance(float elapsedTime)
{
    _Accumulator += std::clamp(elapsedTime, 0.0f, MaxFrameTime);

    int stepCount = 0;
    while (_Accumulator >= DeltaTime)
    {
        _Accumulator -= DeltaTime;
        ++stepCount;
    }

    return stepCount;
}
