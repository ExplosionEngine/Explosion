//
// Created by johnk on 2022/8/15.
//

#include <gtest/gtest.h>

#include <Runtime/World.h>

struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

class VelocitySystem : public Runtime::System {
public:
    VelocitySystem() : Runtime::System() {}
    ~VelocitySystem() override = default;

    void Setup() {}

    void Tick(Runtime::Query<Position, Velocity>& query)
    {
        query.ForEach([](Position& position, Velocity& velocity) -> void {
            velocity.x += position.x;
            velocity.y += position.y;
        });
    }
};

TEST(WorldTest, ComponentBasicTest)
{
    Runtime::World world("TestWorld");

    auto entity = world.CreateEntity();
    world.AddComponent<Position>(entity) = Position { 1.0f, 2.0f };

    auto* position = world.GetComponent<Position>(entity);
    ASSERT_NE(position, nullptr);
    ASSERT_EQ(position->x, 1.0f);
    ASSERT_EQ(position->y, 2.0f);

    world.RemoveComponent<Position>(entity);
    position = world.GetComponent<Position>(entity);
    ASSERT_EQ(position, nullptr);
}

TEST(WorldTest, SystemBasicTest)
{
    Runtime::World world("TestWorld");

    auto entity0 = world.CreateEntity();
    world.AddComponent<Position>(entity0) = Position { 1.0f, 2.0f };
    world.AddComponent<Velocity>(entity0) = Velocity { 0.0f, 0.0f };

    auto entity1 = world.CreateEntity();
    world.AddComponent<Position>(entity1) = Position { 5.0f, 6.0f };
    world.AddComponent<Velocity>(entity1) = Velocity { 1.0f, 2.0f };

    auto* system = new VelocitySystem;
    system->Wait(world.EngineSystems());
    world.AddSystem(system);

    world.Setup();
    world.Tick();
}
