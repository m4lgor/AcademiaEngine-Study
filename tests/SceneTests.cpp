#include "doctest/doctest.h"

#include "Engine/Scene.h"

namespace
{
    class TestObject : public GameObject
    {
    public:
        TestObject() = default;
        explicit TestObject(const olc::vf2d& position) { Position = position; }

        void Update(float elapsedTime) override { ++UpdateCount; }

        int UpdateCount = 0;
    };

    // Spawns a child the first time it is updated, like a gun firing a bullet
    class SpawnerObject : public GameObject
    {
    public:
        explicit SpawnerObject(Scene& scene) : _Scene(scene) {}

        void Update(float elapsedTime) override
        {
            if (!Child.lock())
                Child = _Scene.Spawn<TestObject>();
        }

        std::weak_ptr<TestObject> Child;

    private:
        Scene& _Scene;
    };

    // Destroys itself the first time it is updated
    class SelfDestroyObject : public GameObject
    {
    public:
        explicit SelfDestroyObject(Scene& scene) : _Scene(scene) {}

        void Update(float elapsedTime) override
        {
            ++UpdateCount;
            for (const GameObjectPtr& object : _Scene.GetObjects())
            {
                if (object.get() == this)
                    _Scene.Destroy(object);
            }
        }

        int UpdateCount = 0;

    private:
        Scene& _Scene;
    };
}

TEST_CASE("Scene - Spawn returns the object but only adds it at the end of the frame")
{
    Scene scene;

    std::shared_ptr<TestObject> object = scene.Spawn<TestObject>();

    REQUIRE(object != nullptr);
    CHECK(scene.GetObjectCount() == 0);
    CHECK(IsValid(object));

    scene.FlushPending();

    REQUIRE(scene.GetObjectCount() == 1);
    CHECK(scene.GetObjects()[0] == object);
}

TEST_CASE("Scene - Spawn forwards constructor arguments")
{
    Scene scene;

    std::shared_ptr<TestObject> object = scene.Spawn<TestObject>(olc::vf2d{ 3.0f, -2.0f });

    CHECK(object->Position.x == doctest::Approx(3.0f));
    CHECK(object->Position.y == doctest::Approx(-2.0f));
}

TEST_CASE("Scene - Spawned objects keep their spawn order")
{
    Scene scene;

    std::shared_ptr<TestObject> first = scene.Spawn<TestObject>();
    std::shared_ptr<TestObject> second = scene.Spawn<TestObject>();
    std::shared_ptr<TestObject> third = scene.Spawn<TestObject>();
    scene.FlushPending();

    REQUIRE(scene.GetObjectCount() == 3);
    CHECK(scene.GetObjects()[0] == first);
    CHECK(scene.GetObjects()[1] == second);
    CHECK(scene.GetObjects()[2] == third);
}

TEST_CASE("Scene - Update only reaches objects that were flushed")
{
    Scene scene;

    std::shared_ptr<TestObject> object = scene.Spawn<TestObject>();
    scene.Update(0.016f);
    CHECK(object->UpdateCount == 0);

    scene.FlushPending();
    scene.Update(0.016f);
    CHECK(object->UpdateCount == 1);
}

TEST_CASE("Scene - Spawning from inside Update is safe and takes effect next frame")
{
    Scene scene;

    std::shared_ptr<SpawnerObject> spawner = scene.Spawn<SpawnerObject>(scene);
    scene.FlushPending();

    scene.Update(0.016f);

    std::shared_ptr<TestObject> child = spawner->Child.lock();
    REQUIRE(child != nullptr);
    CHECK(scene.GetObjectCount() == 1);
    CHECK(child->UpdateCount == 0);

    scene.FlushPending();
    CHECK(scene.GetObjectCount() == 2);

    scene.Update(0.016f);
    CHECK(child->UpdateCount == 1);
}

TEST_CASE("Scene - Destroy invalidates immediately and removes at the end of the frame")
{
    Scene scene;

    std::shared_ptr<TestObject> object = scene.Spawn<TestObject>();
    scene.FlushPending();

    scene.Destroy(object);

    CHECK_FALSE(IsValid(object));
    CHECK(scene.GetObjectCount() == 1);

    scene.Update(0.016f);
    CHECK(object->UpdateCount == 0);

    scene.FlushPending();
    CHECK(scene.GetObjectCount() == 0);
}

TEST_CASE("Scene - An object can destroy itself during its own Update")
{
    Scene scene;

    std::shared_ptr<SelfDestroyObject> object = scene.Spawn<SelfDestroyObject>(scene);
    std::shared_ptr<TestObject> other = scene.Spawn<TestObject>();
    scene.FlushPending();

    scene.Update(0.016f);
    scene.FlushPending();

    CHECK(object->UpdateCount == 1);
    REQUIRE(scene.GetObjectCount() == 1);
    CHECK(scene.GetObjects()[0] == other);
}

TEST_CASE("Scene - Destroying an object before it was flushed never adds it")
{
    Scene scene;

    std::shared_ptr<TestObject> object = scene.Spawn<TestObject>();
    scene.Destroy(object);
    scene.FlushPending();

    CHECK(scene.GetObjectCount() == 0);
}

TEST_CASE("Scene - The Scene owns its objects, weak references expire once removed")
{
    Scene scene;

    std::weak_ptr<TestObject> reference = scene.Spawn<TestObject>();
    scene.FlushPending();
    REQUIRE(IsValid(reference));

    scene.Destroy(reference.lock());
    scene.FlushPending();

    CHECK(reference.expired());
    CHECK_FALSE(IsValid(reference));
}

TEST_CASE("Scene - Clear removes every object, including pending spawns")
{
    Scene scene;

    scene.Spawn<TestObject>();
    scene.Spawn<TestObject>();
    scene.FlushPending();
    scene.Spawn<TestObject>();

    scene.Clear();
    scene.FlushPending();

    CHECK(scene.GetObjectCount() == 0);

    scene.Spawn<TestObject>();
    scene.FlushPending();
    CHECK(scene.GetObjectCount() == 1);
}
