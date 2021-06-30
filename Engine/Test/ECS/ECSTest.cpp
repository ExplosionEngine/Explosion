//
// Created by John Kindem on 2021/6/3.
//

#include <string>

#include <gtest/gtest.h>

#include <Explosion/ECS/ECS.h>

using namespace Explosion::ECS;

TEST(ECSTest, EntityComponentTest)
{
    struct PositionComponent {
        float x;
        float y;
        float z;
    };

    Registry registry;

    Entity entity1 = registry.CreateEntity();
    registry.AddComponent<PositionComponent>(entity1, 1.f, 2.f, 3.f);

    Entity entity2 = registry.CreateEntity();
    registry.AddComponent<PositionComponent>(entity2);
    auto* comp = registry.GetComponent<PositionComponent>(entity2);
    comp->x = 4.f;
    comp->y = 5.f;
    comp->z = 6.f;

    Entity entity3 = registry.CreateEntity();

    auto* comp1 = registry.GetComponent<PositionComponent>(entity1);
    ASSERT_NE(comp1, nullptr);
    ASSERT_EQ(comp1->x, 1.f);
    ASSERT_EQ(comp1->y, 2.f);
    ASSERT_EQ(comp1->z, 3.f);

    auto* comp2 = registry.GetComponent<PositionComponent>(entity2);
    ASSERT_NE(comp2, nullptr);
    ASSERT_EQ(comp2->x, 4.f);
    ASSERT_EQ(comp2->y, 5.f);
    ASSERT_EQ(comp2->z, 6.f);

    auto* comp3 = registry.GetComponent<PositionComponent>(entity3);
    ASSERT_EQ(comp3, nullptr);
}

TEST(ECSTest, ViewTest)
{
    struct NameComponent {
        std::string name;
    };

    Registry registry;

    static const uint32_t ENTITY_NUM = 5;

    std::vector<Entity> entities(ENTITY_NUM);
    for (auto i = 0; i < ENTITY_NUM; i++) {
        entities[i] = registry.CreateEntity();
        registry.AddComponent<NameComponent>(entities[i], std::string("entity-") + std::to_string(i));
    }

    uint32_t count = ENTITY_NUM - 1;
    auto view = registry.CreateView<NameComponent>();
    view.Each([&count](const NameComponent& nameComponent) -> void {
        ASSERT_EQ(nameComponent.name, std::string("entity-") + std::to_string(count--));
    });

    view.Each([](NameComponent& nameComponent) -> void {
        nameComponent.name += "-updated";
    });
    for (auto i = 0; i < ENTITY_NUM; i++) {
        ASSERT_EQ(
            registry.GetComponent<NameComponent>(entities[i])->name,
            std::string("entity-") + std::to_string(i) + "-updated"
        );
    }
}
