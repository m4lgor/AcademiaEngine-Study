#include "doctest/doctest.h"

#include "Engine/Scene.h"

namespace
{
    // A Scene that removes what the test left in it, so the leak report stays quiet
    class TestScene : public Scene
    {
    public:
        ~TestScene()
        {
            Clear();
            FlushPending();
        }
    };

    class TestObject : public GameObject
    {
    public:
        TestObject() = default;
        explicit TestObject(const Vector2f& position) { Position = position; }

        void Update(float elapsedTime) override { ++UpdateCount; }
        void FixedUpdate(float fixedDeltaTime) override { ++FixedUpdateCount; LastFixedDeltaTime = fixedDeltaTime; }

        int UpdateCount = 0;
        int FixedUpdateCount = 0;
        float LastFixedDeltaTime = 0.0f;
    };

    class OrderedObject : public GameObject
    {
    public:
        OrderedObject(std::vector<int>& order, int id) : _Order(order), _Id(id) {}

        void Update(float elapsedTime) override { _Order.push_back(_Id); }

    private:
        std::vector<int>& _Order;
        int _Id;
    };

    // Adds a child the first time it is updated, like a gun firing a bullet
    class SpawnerObject : public GameObject
    {
    public:
        explicit SpawnerObject(Scene& scene) : _Scene(scene) {}

        void Update(float elapsedTime) override
        {
            if (!Child)
                Child = _Scene.Add<TestObject>();
        }

        TestObject* Child = nullptr;

    private:
        Scene& _Scene;
    };

    // Removes another object the first time it is updated
    class RemoverObject : public GameObject
    {
    public:
        RemoverObject(Scene& scene, GameObject* target) : _Scene(scene), _Target(target) {}

        void Update(float elapsedTime) override
        {
            _Scene.Remove(_Target);
        }

    private:
        Scene& _Scene;
        GameObject* _Target;
    };
}

TEST_CASE("Scene - Add returns the object but only adds it at the end of the frame")
{
    TestScene scene;

    TestObject* object = scene.Add<TestObject>();

    REQUIRE(object != nullptr);
    CHECK(scene.GetObjectCount() == 0);

    scene.FlushPending();

    CHECK(scene.GetObjectCount() == 1);
    CHECK(object->IsPendingRemove() == false);
}

TEST_CASE("Scene - Add forwards constructor arguments")
{
    TestScene scene;

    TestObject* object = scene.Add<TestObject>(Vector2f{ 3.0f, -2.0f });

    CHECK(object->Position.x == doctest::Approx(3.0f));
    CHECK(object->Position.y == doctest::Approx(-2.0f));
}

TEST_CASE("Scene - FixedUpdate reaches flushed objects with the given delta time")
{
    TestScene scene;

    TestObject* object = scene.Add<TestObject>();
    scene.FixedUpdate(0.5f);
    CHECK(object->FixedUpdateCount == 0);

    scene.FlushPending();
    scene.FixedUpdate(0.5f);
    scene.FixedUpdate(0.5f);

    CHECK(object->FixedUpdateCount == 2);
    CHECK(object->LastFixedDeltaTime == doctest::Approx(0.5f));
    CHECK(object->UpdateCount == 0);
}

TEST_CASE("Scene - FixedUpdate skips objects that were removed")
{
    TestScene scene;

    TestObject* object = scene.Add<TestObject>();
    scene.FlushPending();
    scene.Remove(object);

    scene.FixedUpdate(0.016f);

    CHECK(object->FixedUpdateCount == 0);
}

TEST_CASE("Scene - Added objects keep their add order")
{
    TestScene scene;
    std::vector<int> updateOrder;

    scene.Add<OrderedObject>(updateOrder, 1);
    scene.Add<OrderedObject>(updateOrder, 2);
    scene.Add<OrderedObject>(updateOrder, 3);
    scene.FlushPending();
    scene.Update(0.016f);

    CHECK((updateOrder == std::vector<int>{ 1, 2, 3 }));
}

TEST_CASE("Scene - Update only reaches objects that were flushed")
{
    TestScene scene;

    TestObject* object = scene.Add<TestObject>();
    scene.Update(0.016f);
    CHECK(object->UpdateCount == 0);

    scene.FlushPending();
    scene.Update(0.016f);
    CHECK(object->UpdateCount == 1);
}

TEST_CASE("Scene - Adding from inside Update is safe and takes effect next frame")
{
    TestScene scene;

    SpawnerObject* spawner = scene.Add<SpawnerObject>(scene);
    scene.FlushPending();

    scene.Update(0.016f);

    REQUIRE(spawner->Child != nullptr);
    CHECK(scene.GetObjectCount() == 1);
    CHECK(spawner->Child->UpdateCount == 0);

    scene.FlushPending();
    CHECK(scene.GetObjectCount() == 2);

    scene.Update(0.016f);
    CHECK(spawner->Child->UpdateCount == 1);
}

TEST_CASE("Scene - Remove marks immediately and deletes at the end of the frame")
{
    TestScene scene;

    TestObject* object = scene.Add<TestObject>();
    scene.FlushPending();

    scene.Remove(object);

    CHECK(object->IsPendingRemove());
    CHECK(scene.GetObjectCount() == 1);

    scene.Update(0.016f);
    CHECK(object->UpdateCount == 0);

    scene.FlushPending();
    CHECK(scene.GetObjectCount() == 0);
}

TEST_CASE("Scene - Removing from inside Update is safe, the target is still valid during the frame")
{
    TestScene scene;

    TestObject* target = scene.Add<TestObject>();
    scene.Add<RemoverObject>(scene, target);
    TestObject* other = scene.Add<TestObject>();
    scene.FlushPending();

    scene.Update(0.016f);

    // Same frame: the target was skipped but is not deleted yet
    CHECK(target->IsPendingRemove());
    CHECK(scene.GetObjectCount() == 3);

    scene.FlushPending();
    CHECK(scene.GetObjectCount() == 2);

    scene.Update(0.016f);
    CHECK(other->UpdateCount == 2);
}

TEST_CASE("Scene - Removing an object before it was flushed never adds it")
{
    TestScene scene;

    TestObject* object = scene.Add<TestObject>();
    scene.Remove(object);
    scene.FlushPending();

    CHECK(scene.GetObjectCount() == 0);
}

TEST_CASE("Scene - Remove ignores a null pointer")
{
    TestScene scene;

    scene.Add<TestObject>();
    scene.FlushPending();

    scene.Remove(nullptr);
    scene.FlushPending();

    CHECK(scene.GetObjectCount() == 1);
}

TEST_CASE("Scene - Clear removes every object, including pending additions")
{
    TestScene scene;

    scene.Add<TestObject>();
    scene.Add<TestObject>();
    scene.FlushPending();
    scene.Add<TestObject>();

    scene.Clear();
    scene.FlushPending();

    CHECK(scene.GetObjectCount() == 0);

    scene.Add<TestObject>();
    scene.FlushPending();
    CHECK(scene.GetObjectCount() == 1);
}

// The next two tests print the leak report on purpose
TEST_CASE("Scene - ReportLeaks counts the objects that were added but never removed")
{
    Scene scene;

    TestObject* removed = scene.Add<TestObject>();
    scene.Add<TestObject>();
    scene.Add<TestObject>();
    scene.FlushPending();
    scene.Remove(removed);

    CHECK(scene.ReportLeaks() == 2);
    CHECK(scene.GetObjectCount() == 0);
}

TEST_CASE("Scene - ReportLeaks finds nothing when everything was removed")
{
    Scene scene;

    TestObject* first = scene.Add<TestObject>();
    TestObject* second = scene.Add<TestObject>();
    scene.FlushPending();
    scene.Remove(first);
    scene.Remove(second);

    CHECK(scene.ReportLeaks() == 0);
}
