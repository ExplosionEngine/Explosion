//
// Created by johnk on 2024/12/25.
//

#include <WorldTest.h>
#include <Test/Test.h>
#include <Runtime/World.h>
using namespace Runtime;

struct WorldTest : testing::Test {
    void SetUp() override
    {
        EngineInitParams engineInitParams {};
        engineInitParams.rhiType = RHI::GetAbbrStringByType(RHI::RHIType::dummy);

        EngineHolder::Load("RuntimeTest", engineInitParams);
        engine = &EngineHolder::Get();
    }

    void TearDown() override
    {
        EngineHolder::Unload();
    }

    Engine* engine;
};

Position::Position()
    : x(0.0f)
    , y(0.0f)
{
}

Position::Position(float inX, float inY)
    : x(inX)
    , y(inY)
{
}

bool Position::operator==(const Position& inRhs) const
{
    return Common::CompareNumber(x, inRhs.x)
        && Common::CompareNumber(y, inRhs.y);
}

Velocity::Velocity()
    : x(0.0f)
    , y(0.0f)
{
}

Velocity::Velocity(float inX, float inY)
    : x(inX)
    , y(inY)
{
}

bool Velocity::operator==(const Velocity& inRhs) const
{
    return Common::CompareNumber(x, inRhs.x)
        && Common::CompareNumber(y, inRhs.y);
}

BasicTest_MotionSystem::BasicTest_MotionSystem(ECRegistry& inRegistry, const Runtime::SystemSetupContext& inContext)
    : System(inRegistry, inContext)
{
    auto entity0 = registry.Create();
    registry.Emplace<Position>(entity0, 1.0f, 2.0f);
    registry.Emplace<Velocity>(entity0, 0.5f, 1.0f);

    auto entity1 = registry.Create();
    registry.Emplace<Position>(entity1, 0.0f, 1.0f);
    registry.Emplace<Velocity>(entity1, 1.0f, 0.0f);

    auto& [entities] = registry.GEmplace<GBasicTest_ExpectVerifyResult>();
    entities = {
        { entity0, Position(1.5f, 3.0f) },
        { entity1, Position(1.0f, 1.0f) }
    };
}

BasicTest_MotionSystem::~BasicTest_MotionSystem() = default;

void BasicTest_MotionSystem::Tick(float inDeltaTimeSeconds)
{
    auto& [entities] = registry.GGet<GBasicTest_ExpectVerifyResult>();

    const auto view = registry.View<Position, Velocity>();
    view.Each([&](Entity entity, Position& position, Velocity& velocity) -> void {
        auto& expectPos = entities.at(entity);
        position.x += velocity.x;
        position.y += velocity.y;
        ASSERT_EQ(expectPos, position);

        expectPos.x = position.x + velocity.x;
        expectPos.y = position.y + velocity.y;
    });
    registry.GNotifyUpdated<GBasicTest_ExpectVerifyResult>();
}

TEST_F(WorldTest, BasicTest)
{
    SystemGraph systemGraph;
    auto& mainGroup = systemGraph.AddGroup("MainGroup", SystemExecuteStrategy::sequential);
    mainGroup.EmplaceSystemDyn(&BasicTest_MotionSystem::GetStaticClass());

    World world("TestWorld", nullptr, PlayType::game);
    world.SetSystemGraph(systemGraph);
    world.Play();
    for (auto i = 0; i < 5; i++) {
        engine->Tick(0.0167f);
    }
    world.Stop();
}

ConcurrentTest_SystemA::ConcurrentTest_SystemA(Runtime::ECRegistry& inRegistry, const Runtime::SystemSetupContext& inContext)
    : System(inRegistry, inContext)
{
}

ConcurrentTest_SystemA::~ConcurrentTest_SystemA() = default;

void ConcurrentTest_SystemA::Tick(float inDeltaTimeSeconds)
{
    auto& context = registry.GGet<GConcurrentTest_Context>();
    context.a = 1;
}

ConcurrentTest_SystemB::ConcurrentTest_SystemB(Runtime::ECRegistry& inRegistry, const Runtime::SystemSetupContext& inContext)
    : System(inRegistry, inContext)
{
}

ConcurrentTest_SystemB::~ConcurrentTest_SystemB() = default;

void ConcurrentTest_SystemB::Tick(float inDeltaTimeSeconds)
{
    auto& context = registry.GGet<GConcurrentTest_Context>();
    context.b = 2;
}

ConcurrentTest_VerifySystem::ConcurrentTest_VerifySystem(Runtime::ECRegistry& inRegistry, const Runtime::SystemSetupContext& inContext)
    : System(inRegistry, inContext)
{
    auto& context = registry.GEmplace<GConcurrentTest_Context>();
    context.a = 0;
    context.b = 0;
    context.sum = 0;
    context.tickCount = 0;
}

ConcurrentTest_VerifySystem::~ConcurrentTest_VerifySystem() = default;

void ConcurrentTest_VerifySystem::Tick(float inDeltaTimeSeconds)
{
    auto& context = registry.GGet<GConcurrentTest_Context>();
    context.sum = context.sum + context.a + context.b;
    context.a = 0;
    context.b = 0;
    context.tickCount++;
    ASSERT_EQ(context.sum, 3 * context.tickCount);
}

TEST_F(WorldTest, ConcurrentTest)
{
    SystemGraph systemGraph;
    auto& ConcurrentGroup = systemGraph.AddGroup("ConcurrentGroup", SystemExecuteStrategy::concurrent);
    ConcurrentGroup.EmplaceSystem<ConcurrentTest_SystemA>();
    ConcurrentGroup.EmplaceSystem<ConcurrentTest_SystemB>();
    auto& verifyGroup = systemGraph.AddGroup("VerifyGroup", SystemExecuteStrategy::sequential);
    verifyGroup.EmplaceSystem<ConcurrentTest_VerifySystem>();

    World world("TestWorld", nullptr, PlayType::game);
    world.SetSystemGraph(systemGraph);
    world.Play();
    for (auto i = 0; i < 5; i++) {
        engine->Tick(0.0167f);
    }
    world.Stop();
}
