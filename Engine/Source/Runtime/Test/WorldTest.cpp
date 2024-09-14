//
// Created by johnk on 2024/8/20.
//

#include <WorldTest.h>
#include <Runtime/Engine.h>
#include <Test/Test.h>

struct WorldTest : testing::Test {
    void SetUp() override
    {
        Runtime::EngineHolder::Load("RuntimeTest", {});
    }
};

void BasicSetupSystem::Execute(Runtime::Commands& commands, const Runtime::WorldStart&) // NOLINT
{
    for (auto i = 0; i < 5; i++) {
        const Runtime::Entity entity = commands.CreateEntity();
        commands.EmplaceComp<Position>(entity);
        commands.EmplaceComp<Velocity>(entity);
    }
}

StartVerify BasicTickSystem::Execute(Runtime::Commands& commands, const Runtime::WorldTick&) // NOLINT
{
    auto& count = commands.GetState<IterTimeCount>();
    count.value += 1;

    auto view = commands.View<Position, Velocity>();
    view.Each([](Position& position, Velocity& velocity) -> void {
        position.x += velocity.x;
        position.y += velocity.y;
    });

    return {};
}

void PositionVerifySystem::Execute(Runtime::Commands& commands, const StartVerify&) // NOLINT
{
    const IterTimeCount& count = commands.GetState<IterTimeCount>();

    auto view = commands.View<Position, Velocity>();
    view.Each([&](const Position& position, const Velocity& velocity) -> void {
        ASSERT_EQ(position.x, velocity.x * count.value);
    });
}

TEST_F(WorldTest, ECSBasic)
{
    Runtime::World world;
    world.AddSystem<BasicSetupSystem>();
    world.AddSystem<PositionVerifySystem>();
    world.Start();
    world.Tick(10.f);
}
