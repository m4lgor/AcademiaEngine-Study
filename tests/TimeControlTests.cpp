#include "doctest/doctest.h"

#include "Engine/TimeControl.h"

TEST_CASE("TimeControl - running gives the real frame time")
{
    TimeControl time;
    CHECK(time.Tick(0.02f));
    CHECK(time.GetElapsedTime() == doctest::Approx(0.02f));
}

TEST_CASE("TimeControl - paused skips gameplay")
{
    TimeControl time;
    time.SetPaused(true);
    CHECK_FALSE(time.Tick(0.02f));
    CHECK(time.GetElapsedTime() == 0.0f);
}

TEST_CASE("TimeControl - a step while paused runs exactly one fixed step")
{
    TimeControl time;
    time.SetPaused(true);
    time.StepOneFrame();

    CHECK(time.Tick(0.002f));
    CHECK(time.GetElapsedTime() == doctest::Approx(TimeControl::StepDeltaTime));

    // Only one frame
    CHECK_FALSE(time.Tick(0.002f));
}

TEST_CASE("TimeControl - a step lasts as long as one fixed step")
{
    CHECK(TimeControl::StepDeltaTime == FixedTimestep::DeltaTime);
}

TEST_CASE("TimeControl - step is ignored while running")
{
    TimeControl time;
    time.StepOneFrame();
    CHECK(time.Tick(0.02f));

    // Pausing later does not replay the old request
    time.SetPaused(true);
    CHECK_FALSE(time.Tick(0.02f));
}

TEST_CASE("TimeControl - toggle pause")
{
    TimeControl time;
    time.TogglePause();
    CHECK(time.IsPaused());
    time.TogglePause();
    CHECK_FALSE(time.IsPaused());
}
