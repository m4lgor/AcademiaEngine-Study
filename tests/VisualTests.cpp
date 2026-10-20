#include "doctest/doctest.h"

#include "VisualTest.h"
#include "Game/ExampleGameObject.h"

// These tests open the engine window. They only run with --visual (see TestMain.cpp).

namespace
{
    // Grey lines every world unit, brighter axes through the origin
    class GridObject : public GameObject
    {
    public:
        void Draw(AcademiaEngine& engine) override
        {
            constexpr int halfExtent = 40;
            for (int i = -halfExtent; i <= halfExtent; ++i)
            {
                const olc::Pixel color = (i == 0) ? olc::GREY : olc::VERY_DARK_GREY;
                const float line = static_cast<float>(i);
                const float extent = static_cast<float>(halfExtent);
                engine.DrawLine(engine.ConvertWorldPositionToPixels({ line, -extent }), engine.ConvertWorldPositionToPixels({ line, extent }), color);
                engine.DrawLine(engine.ConvertWorldPositionToPixels({ -extent, line }), engine.ConvertWorldPositionToPixels({ extent, line }), color);
            }
        }
    };

    // Spawns a green child to its right during its first Update, like a gun firing a bullet
    class SpawnerObject : public ExampleGameObject
    {
    public:
        void Update(float elapsedTime) override
        {
            if (_HasSpawned)
                return;

            _HasSpawned = true;
            std::shared_ptr<ExampleGameObject> child = OwnerScene->Spawn<ExampleGameObject>();
            child->Position = Position + olc::vf2d{ 1.5f, 0.0f };
            child->Color = olc::GREEN;
            Child = child;
        }

        Scene* OwnerScene = nullptr;
        std::weak_ptr<ExampleGameObject> Child;

    private:
        bool _HasSpawned = false;
    };

    std::shared_ptr<ExampleGameObject> SpawnCircle(VisualTestEngine& engine, const olc::vf2d& position, const olc::Pixel& color)
    {
        std::shared_ptr<ExampleGameObject> circle = engine.GetScene().Spawn<ExampleGameObject>();
        circle->Position = position;
        circle->Color = color;
        return circle;
    }

    void CheckColorAt(VisualTestEngine& engine, const olc::vf2d& worldPosition, const olc::Pixel& expected)
    {
        const olc::Pixel actual = engine.GetPixelAtWorld(worldPosition);
        INFO("at world (", worldPosition.x, ", ", worldPosition.y, ")");
        CHECK(actual == expected);
    }
}

TEST_SUITE("Visual")
{
    TEST_CASE("Visual - Spawn and destroy game objects")
    {
        // 5x5 grid, 2 units apart
        auto gridPosition = [](int column, int row) { return olc::vf2d{ (column - 2) * 2.0f, (row - 2) * 2.0f }; };
        std::vector<std::weak_ptr<ExampleGameObject>> circles;
        std::weak_ptr<SpawnerObject> spawner;

        RunVisualTest("Spawn and destroy game objects", {
            {
                "Spawn a 5x5 grid of circles",
                [&](VisualTestEngine& engine)
                {
                    for (int row = 0; row < 5; ++row)
                        for (int column = 0; column < 5; ++column)
                            circles.push_back(SpawnCircle(engine, gridPosition(column, row), olc::WHITE));
                },
                [&](VisualTestEngine& engine)
                {
                    CHECK(engine.GetScene().GetObjectCount() == 25);
                    for (int row = 0; row < 5; ++row)
                        for (int column = 0; column < 5; ++column)
                            CheckColorAt(engine, gridPosition(column, row), olc::WHITE);
                }
            },
            {
                "Destroy every other circle",
                [&](VisualTestEngine& engine)
                {
                    for (int row = 0; row < 5; ++row)
                        for (int column = 0; column < 5; ++column)
                            if ((row + column) % 2 == 1)
                                engine.GetScene().Destroy(circles[row * 5 + column].lock());
                },
                [&](VisualTestEngine& engine)
                {
                    CHECK(engine.GetScene().GetObjectCount() == 13);
                    for (int row = 0; row < 5; ++row)
                        for (int column = 0; column < 5; ++column)
                        {
                            const bool destroyed = (row + column) % 2 == 1;
                            CHECK(IsValid(circles[row * 5 + column]) == !destroyed);
                            CheckColorAt(engine, gridPosition(column, row), destroyed ? olc::BLACK : olc::WHITE);
                        }
                }
            },
            {
                "Red spawner spawns a green circle from its Update",
                [&](VisualTestEngine& engine)
                {
                    std::shared_ptr<SpawnerObject> object = engine.GetScene().Spawn<SpawnerObject>();
                    object->OwnerScene = &engine.GetScene();
                    object->Position = { -2.0f, -7.0f };
                    object->Color = olc::RED;
                    spawner = object;
                },
                [&](VisualTestEngine& engine)
                {
                    CHECK(engine.GetScene().GetObjectCount() == 15);
                    CheckColorAt(engine, { -2.0f, -7.0f }, olc::RED);
                    CheckColorAt(engine, { -0.5f, -7.0f }, olc::GREEN);
                }
            },
            {
                "Clear the scene",
                [&](VisualTestEngine& engine)
                {
                    engine.GetScene().Clear();
                },
                [&](VisualTestEngine& engine)
                {
                    CHECK(engine.GetScene().GetObjectCount() == 0);
                    CHECK_FALSE(IsValid(spawner));
                    CheckColorAt(engine, gridPosition(2, 2), olc::BLACK);
                    CheckColorAt(engine, { -2.0f, -7.0f }, olc::BLACK);
                }
            },
        });
    }

    TEST_CASE("Visual - Camera pan and zoom")
    {
        // Reference scene: grid, white circle at the origin, one colored circle 2 units away on each side
        const olc::vf2d right = { 2.0f, 0.0f };
        const olc::vf2d left = { -2.0f, 0.0f };
        const olc::vf2d up = { 0.0f, 2.0f };
        const olc::vf2d down = { 0.0f, -2.0f };

        auto setCamera = [](VisualTestEngine& engine, const olc::vf2d& position, float zoom)
        {
            engine.GetCamera().Position = position;
            engine.GetCamera().Zoom = zoom;
        };
        auto checkCenterColor = [](VisualTestEngine& engine, const olc::Pixel& expected)
        {
            INFO("at the screen center");
            CHECK(engine.GetDrawTarget()->GetPixel(512, 512) == expected);
        };

        RunVisualTest("Camera pan and zoom", {
            {
                "Reference: camera at the origin, zoom 1",
                [&](VisualTestEngine& engine)
                {
                    engine.GetScene().Spawn<GridObject>();
                    SpawnCircle(engine, { 0.0f, 0.0f }, olc::WHITE);
                    SpawnCircle(engine, right, olc::RED);
                    SpawnCircle(engine, left, olc::BLUE);
                    SpawnCircle(engine, up, olc::GREEN);
                    SpawnCircle(engine, down, olc::YELLOW);
                },
                [&](VisualTestEngine& engine)
                {
                    checkCenterColor(engine, olc::WHITE);
                    CHECK(engine.GetDrawTarget()->GetPixel(612, 512) == olc::RED);
                    CHECK(engine.GetDrawTarget()->GetPixel(512, 412) == olc::GREEN);
                }
            },
            {
                "Pan right: red circle comes to the center",
                [&](VisualTestEngine& engine) { setCamera(engine, right, 1.0f); },
                [&](VisualTestEngine& engine) { checkCenterColor(engine, olc::RED); }
            },
            {
                "Pan left: blue circle comes to the center",
                [&](VisualTestEngine& engine) { setCamera(engine, left, 1.0f); },
                [&](VisualTestEngine& engine) { checkCenterColor(engine, olc::BLUE); }
            },
            {
                "Pan up: green circle comes to the center",
                [&](VisualTestEngine& engine) { setCamera(engine, up, 1.0f); },
                [&](VisualTestEngine& engine) { checkCenterColor(engine, olc::GREEN); }
            },
            {
                "Pan down: yellow circle comes to the center",
                [&](VisualTestEngine& engine) { setCamera(engine, down, 1.0f); },
                [&](VisualTestEngine& engine) { checkCenterColor(engine, olc::YELLOW); }
            },
            {
                "Zoom in x2: circles twice as big and far",
                [&](VisualTestEngine& engine) { setCamera(engine, { 0.0f, 0.0f }, 2.0f); },
                [&](VisualTestEngine& engine)
                {
                    checkCenterColor(engine, olc::WHITE);
                    // Red circle: 2 units * 100 px, radius 0.4 * 100 = 40 px
                    CHECK(engine.GetDrawTarget()->GetPixel(712, 512) == olc::RED);
                    CHECK(engine.GetDrawTarget()->GetPixel(712 + 35, 512) == olc::RED);
                }
            },
            {
                "Zoom out x0.5: circles half as big and close",
                [&](VisualTestEngine& engine) { setCamera(engine, { 0.0f, 0.0f }, 0.5f); },
                [&](VisualTestEngine& engine)
                {
                    checkCenterColor(engine, olc::WHITE);
                    // Red circle: 2 units * 25 px, radius 0.4 * 25 = 10 px
                    CHECK(engine.GetDrawTarget()->GetPixel(562, 512) == olc::RED);
                    CHECK(engine.GetDrawTarget()->GetPixel(562 + 15, 512) != olc::RED);
                }
            },
            {
                "Pan and zoom: camera on the red circle, zoom x3",
                [&](VisualTestEngine& engine) { setCamera(engine, right, 3.0f); },
                [&](VisualTestEngine& engine)
                {
                    checkCenterColor(engine, olc::RED);
                    // White circle at the origin: 2 units * 150 px to the left
                    CHECK(engine.GetDrawTarget()->GetPixel(212, 512) == olc::WHITE);
                }
            },
        });
    }
}
