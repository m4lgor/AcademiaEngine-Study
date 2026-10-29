#include "doctest/doctest.h"

#include "Engine/GameBase.h"

namespace
{
    class CountingObject : public GameObject
    {
    public:
        void Update(float elapsedTime) override { ++UpdateCount; }
        void FixedUpdate(float fixedDeltaTime) override { ++FixedUpdateCount; }

        int UpdateCount = 0;
        int FixedUpdateCount = 0;
    };

    class EmptyGame : public GameBase
    {
    public:
        void Initialize(AcademiaEngine* engineContext) override {}
        void Uninitialize() override {}
    };

    // Overrides Update and forgets to call the base version
    class ForgetfulGame : public EmptyGame
    {
    public:
        void Update(Scene& scene, float elapsedTime) override {}
    };

    class TestScene : public Scene
    {
    public:
        ~TestScene()
        {
            Clear();
            FlushPending();
        }
    };
}

TEST_CASE("GameBase - Update and FixedUpdate update the objects of the Scene by default")
{
    TestScene scene;
    CountingObject* object = scene.Add<CountingObject>();
    scene.FlushPending();

    EmptyGame game;
    game.Update(scene, 0.016f);
    game.FixedUpdate(scene, 0.016f);
    game.FixedUpdate(scene, 0.016f);

    CHECK(object->UpdateCount == 1);
    CHECK(object->FixedUpdateCount == 2);
}

TEST_CASE("GameBase - an override that does not call the base version updates nothing")
{
    TestScene scene;
    CountingObject* object = scene.Add<CountingObject>();
    scene.FlushPending();

    ForgetfulGame game;
    game.Update(scene, 0.016f);

    CHECK(object->UpdateCount == 0);
}
