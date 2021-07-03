//
// Created by John Kindem on 2021/6/24.
//

#include <gtest/gtest.h>

#include <Explosion/ECS/ECS.h>
#include <Explosion/World/World.h>

using namespace Explosion;
using namespace Explosion::ECS;

TEST(WorldTest, SystemTest1)
{
    struct NameComponent {
        std::string name;
    };

    World world;
    auto& registry = world.GetRegistry();

    Entity entity1 = registry.CreateEntity();
    registry.AddComponent<NameComponent>(entity1, "entity1");

    Entity entity2 = registry.CreateEntity();
    registry.AddComponent<NameComponent>(entity2, "entity2");

    SystemGroup userSystemGroup {};
    userSystemGroup.name = "userGroup";
    userSystemGroup.priority = 10;
    userSystemGroup.systems["nameSystem"] = [](Registry& registry, float time) -> void {
        auto view = registry.CreateView<NameComponent>();
        view.Each([](NameComponent& nameComp) -> void {
            nameComp.name += "-updated";
        });
    };

    world.AddSystemGroups(userSystemGroup);
    world.Tick(.33f);
    ASSERT_EQ(registry.GetComponent<NameComponent>(entity1)->name, "entity1-updated");
    ASSERT_EQ(registry.GetComponent<NameComponent>(entity2)->name, "entity2-updated");

    world.Tick(.66f);
    ASSERT_EQ(registry.GetComponent<NameComponent>(entity1)->name, "entity1-updated-updated");
    ASSERT_EQ(registry.GetComponent<NameComponent>(entity2)->name, "entity2-updated-updated");
}

TEST(WorldTest, SystemPriorityTest)
{
    struct NameComponent {
        std::string name;
    };

    World world;
    auto& registry = world.GetRegistry();

    Entity entity = registry.CreateEntity();
    registry.AddComponent<NameComponent>(entity, "entity");

    SystemGroup internSystemGroup {};
    internSystemGroup.name = "internGroup";
    internSystemGroup.priority = 100;
    internSystemGroup.systems["internNameSystem"] = [](Registry& registry, float time) -> void {
        auto view = registry.CreateView<NameComponent>();
        view.Each([](NameComponent& nameComp) -> void {
            nameComp.name += "-intern";
        });
    };

    SystemGroup userSystemGroup {};
    userSystemGroup.name = "userGroup";
    userSystemGroup.priority = 10;
    userSystemGroup.systems["nameSystem"] = [](Registry& registry, float time) -> void {
        auto view = registry.CreateView<NameComponent>();
        view.Each([](NameComponent& nameComp) -> void {
            nameComp.name += "-user";
        });
    };

    world.AddSystemGroups(internSystemGroup);
    world.AddSystemGroups(userSystemGroup);
    world.Tick(.55f);
    ASSERT_EQ(registry.GetComponent<NameComponent>(entity)->name, "entity-intern-user");
}

TEST(WorldTest, SystemDependenciesTest)
{
    struct SharedComponent {
        uint32_t input;
        uint32_t transfer;
    };

    struct OutputComponent {
        uint32_t value;
    };

    World world;
    auto& registry = world.GetRegistry();

    Entity entity1 = registry.CreateEntity();
    registry.AddComponent<SharedComponent>(entity1, 1u, 0u);
    registry.AddComponent<OutputComponent>(entity1, 0u);

    Entity entity2 = registry.CreateEntity();
    registry.AddComponent<SharedComponent>(entity2, 3u, 0u);
    registry.AddComponent<OutputComponent>(entity2, 0u);

    SystemGroup systemGroup {};
    systemGroup.name = "group1";
    systemGroup.priority = 1;
    systemGroup.systems["middleSystem"] = [](Registry& registry, float time) -> void {
        auto view = registry.CreateView<SharedComponent>();
        view.Each([](SharedComponent& sharedComp) -> void {
            sharedComp.transfer = sharedComp.input * 4;
        });
    };
    systemGroup.systems["outputSystem"] = [](Registry& registry, float time) -> void {
        auto view = registry.CreateView<SharedComponent, OutputComponent>();
        view.Each([](SharedComponent& sharedComp, OutputComponent& outputComp) -> void {
            outputComp.value = sharedComp.transfer + 3;
        });
    };
    systemGroup.dependencies["outputSystem"] = "middleSystem";

    world.AddSystemGroups(systemGroup);
    world.Tick(.16f);
    ASSERT_EQ(registry.GetComponent<SharedComponent>(entity1)->input, 1);
    ASSERT_EQ(registry.GetComponent<SharedComponent>(entity1)->transfer, 4);
    ASSERT_EQ(registry.GetComponent<OutputComponent>(entity1)->value, 7);
    ASSERT_EQ(registry.GetComponent<SharedComponent>(entity2)->input, 3);
    ASSERT_EQ(registry.GetComponent<SharedComponent>(entity2)->transfer, 12);
    ASSERT_EQ(registry.GetComponent<OutputComponent>(entity2)->value, 15);
}
