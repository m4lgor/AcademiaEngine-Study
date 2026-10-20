#include "doctest/doctest.h"

#include "Engine/Camera.h"

namespace
{
    const olc::vi2d ScreenSize = { 1024, 1024 };
    const olc::vi2d ScreenCenter = { 512, 512 };
}

TEST_CASE("Camera - Default camera puts the world origin at the screen center")
{
    Camera camera;

    CHECK(camera.WorldToScreen({ 0.0f, 0.0f }, ScreenSize) == ScreenCenter);
}

TEST_CASE("Camera - World Y points up, screen Y points down")
{
    Camera camera;

    // 1 unit = 50 pixels by default
    CHECK(camera.WorldToScreen({ 1.0f, 0.0f }, ScreenSize) == olc::vi2d{ 562, 512 });
    CHECK(camera.WorldToScreen({ 0.0f, 1.0f }, ScreenSize) == olc::vi2d{ 512, 462 });
}

TEST_CASE("Camera - Moving the camera to the sides shifts objects the other way on screen")
{
    Camera camera;
    const olc::vf2d objectPosition = { 0.0f, 0.0f };

    SUBCASE("Right")
    {
        camera.Position = { 2.0f, 0.0f };
        CHECK(camera.WorldToScreen(objectPosition, ScreenSize) == olc::vi2d{ 412, 512 });
    }
    SUBCASE("Left")
    {
        camera.Position = { -2.0f, 0.0f };
        CHECK(camera.WorldToScreen(objectPosition, ScreenSize) == olc::vi2d{ 612, 512 });
    }
    SUBCASE("Up")
    {
        camera.Position = { 0.0f, 2.0f };
        CHECK(camera.WorldToScreen(objectPosition, ScreenSize) == olc::vi2d{ 512, 612 });
    }
    SUBCASE("Down")
    {
        camera.Position = { 0.0f, -2.0f };
        CHECK(camera.WorldToScreen(objectPosition, ScreenSize) == olc::vi2d{ 512, 412 });
    }
}

TEST_CASE("Camera - What the camera looks at is always at the screen center")
{
    Camera camera;
    camera.Position = { 7.3f, -4.1f };
    camera.Zoom = 3.0f;

    CHECK(camera.WorldToScreen(camera.Position, ScreenSize) == ScreenCenter);
    CHECK(camera.ScreenToWorld(ScreenCenter, ScreenSize).x == doctest::Approx(7.3f));
    CHECK(camera.ScreenToWorld(ScreenCenter, ScreenSize).y == doctest::Approx(-4.1f));
}

TEST_CASE("Camera - Zoom scales distances around the camera position")
{
    Camera camera;
    const olc::vf2d objectPosition = { 1.0f, 1.0f };

    SUBCASE("Zoom in")
    {
        camera.Zoom = 2.0f;
        CHECK(camera.GetScale() == doctest::Approx(100.0f));
        CHECK(camera.WorldToScreen(objectPosition, ScreenSize) == olc::vi2d{ 612, 412 });
    }
    SUBCASE("Zoom out")
    {
        camera.Zoom = 0.5f;
        CHECK(camera.GetScale() == doctest::Approx(25.0f));
        CHECK(camera.WorldToScreen(objectPosition, ScreenSize) == olc::vi2d{ 537, 487 });
    }
    SUBCASE("Zoom around a moved camera")
    {
        camera.Position = { 1.0f, 0.0f };
        camera.Zoom = 2.0f;
        // Object is 1 unit above the camera: 100 pixels above the center
        CHECK(camera.WorldToScreen(objectPosition, ScreenSize) == olc::vi2d{ 512, 412 });
    }
}

TEST_CASE("Camera - Lengths follow the zoom but not the camera position")
{
    Camera camera;
    camera.Position = { 10.0f, 10.0f };

    CHECK(camera.WorldToScreenLength(0.4f) == doctest::Approx(20.0f));

    camera.Zoom = 2.0f;
    CHECK(camera.WorldToScreenLength(0.4f) == doctest::Approx(40.0f));
    CHECK(camera.ScreenToWorldLength(40.0f) == doctest::Approx(0.4f));
}

TEST_CASE("Camera - ScreenToWorld is the inverse of WorldToScreen")
{
    Camera camera;
    camera.Position = { -3.5f, 2.25f };
    camera.Zoom = 1.5f;

    const olc::vi2d pixel = { 300, 700 };
    const olc::vf2d world = camera.ScreenToWorld(pixel, ScreenSize);

    CHECK(camera.WorldToScreen(world, ScreenSize) == pixel);
}

TEST_CASE("Camera - Positions are rounded to the nearest pixel")
{
    Camera camera;

    // 0.019 units = 0.95 pixels: rounds to 1, truncation would give 0
    CHECK(camera.WorldToScreen({ 0.019f, 0.0f }, ScreenSize).x == 513);
    CHECK(camera.WorldToScreen({ -0.019f, 0.0f }, ScreenSize).x == 511);
}

TEST_CASE("Camera - A zero or negative zoom does not break the conversion")
{
    Camera camera;

    for (const float zoom : { 0.0f, -1.0f })
    {
        CAPTURE(zoom);
        camera.Zoom = zoom;

        CHECK(camera.GetScale() > 0.0f);
        CHECK(camera.WorldToScreen({ 0.0f, 0.0f }, ScreenSize) == ScreenCenter);
    }
}
