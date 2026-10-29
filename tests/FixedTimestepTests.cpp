#include "doctest/doctest.h"

#include "Engine/FixedTimestep.h"

TEST_CASE("FixedTimestep - a frame shorter than a step runs none and keeps the time")
{
    FixedTimestep timestep;

    CHECK(timestep.Advance(0.01f) == 0);
    CHECK(timestep.GetAlpha() == doctest::Approx(0.01f / FixedTimestep::DeltaTime));

    // The kept time adds up with the next frame
    CHECK(timestep.Advance(0.01f) == 1);
}

TEST_CASE("FixedTimestep - a long frame runs several steps")
{
    FixedTimestep timestep;

    CHECK(timestep.Advance(0.055f) == 3);
}

TEST_CASE("FixedTimestep - no time is lost: steps plus leftover equal the real time")
{
    FixedTimestep timestep;
    int totalSteps = 0;
    float totalTime = 0.0f;

    for (int frame = 0; frame < 500; ++frame)
    {
        const float frameTime = 0.004f + 0.0001f * static_cast<float>(frame % 7);
        totalSteps += timestep.Advance(frameTime);
        totalTime += frameTime;
    }

    const float simulated = static_cast<float>(totalSteps) * FixedTimestep::DeltaTime;
    const float leftover = timestep.GetAlpha() * FixedTimestep::DeltaTime;
    CHECK(simulated + leftover == doctest::Approx(totalTime).epsilon(0.001));
}

TEST_CASE("FixedTimestep - a frame of exactly one step runs exactly one step")
{
    FixedTimestep timestep;
    timestep.Advance(0.01f);
    const float alphaBefore = timestep.GetAlpha();

    // This is how a single step while paused works: the leftover does not change
    for (int i = 0; i < 20; ++i)
        CHECK(timestep.Advance(FixedTimestep::DeltaTime) == 1);

    CHECK(timestep.GetAlpha() == doctest::Approx(alphaBefore).epsilon(0.01));
}

TEST_CASE("FixedTimestep - a very long frame is capped")
{
    FixedTimestep timestep;

    const int stepCount = timestep.Advance(10.0f);

    CHECK(stepCount >= 14);
    CHECK(static_cast<float>(stepCount) * FixedTimestep::DeltaTime <= FixedTimestep::MaxFrameTime);
}

TEST_CASE("FixedTimestep - a negative frame time is ignored")
{
    FixedTimestep timestep;

    CHECK(timestep.Advance(-1.0f) == 0);
    CHECK(timestep.GetAlpha() == 0.0f);
}
