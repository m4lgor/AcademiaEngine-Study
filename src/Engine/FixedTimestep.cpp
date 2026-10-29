#include "FixedTimestep.h"

int FixedTimestep::Advance(float elapsedTime)
{
    // TODO (class): write the accumulator.
    //  - Add the frame time to _Accumulator. Cut a very long frame to MaxFrameTime first,
    //    so the game does not try to catch up with hundreds of steps at once.
    //  - Count how many steps of DeltaTime the accumulator holds, and remove them from it.
    //  - Return that count: the engine runs one FixedUpdate for each step.

    return 0;
}
