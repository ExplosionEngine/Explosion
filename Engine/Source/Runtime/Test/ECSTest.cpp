//
// Created by johnk on 2024/12/9.
//

#include <ECSTest.h>

TEST(ECSTest, EntityTest)
{
    ECRegistry registry;
    const auto entity0 = registry.Create();
    const auto entity1 = registry.Create();
    ASSERT_EQ(registry.Size(), 2);
    ASSERT_TRUE(registry.Valid(entity0));
    ASSERT_TRUE(registry.Valid(entity1));
    ASSERT_FALSE(registry.Valid(99));

    std::unordered_set cur = { entity0, entity1 };
    for (const auto e : registry) {
        ASSERT_TRUE(cur.contains(e));
    }

    const auto entity2 = registry.Create();
    const auto entity3 = registry.Create();
    registry.Destroy(entity0);
    const auto entity4 = registry.Create();
    cur = { entity1, entity2, entity3, entity4 };

    registry.Each([&](const auto e) -> void {
        ASSERT_TRUE(cur.contains(e));
    });
}

TEST(ECSTest, ComponentStaticTest)
{
    // ECRegistry registry;
    // const auto entity0 = registry.Create();
    // const auto entity1 = registry.Create();
    //
    // registry.Emplace<CompA>(entity0, 1);
    // registry.Emplace<CompA>(entity1, 2);
    // ASSERT_TRUE(registry.Has<CompA>(entity0));
    // ASSERT_TRUE(registry.Has<CompA>(entity1));

    // TODO
}

// TODO
