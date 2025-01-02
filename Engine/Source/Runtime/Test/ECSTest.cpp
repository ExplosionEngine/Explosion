//
// Created by johnk on 2024/12/9.
//

#include <ECSTest.h>
#include <Test/Test.h>

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
    ECRegistry registry;
    const auto entity0 = registry.Create();
    const auto entity1 = registry.Create();

    registry.Emplace<CompA>(entity0, 1);
    registry.Emplace<CompA>(entity1, 2);
    ASSERT_TRUE(registry.Has<CompA>(entity0));
    ASSERT_TRUE(registry.Has<CompA>(entity1));
    ASSERT_EQ(registry.Find<CompA>(entity0)->value, 1);
    ASSERT_EQ(registry.Find<CompA>(entity1)->value, 2);
    ASSERT_EQ(registry.Get<CompA>(entity0).value, 1);
    ASSERT_EQ(registry.Get<CompA>(entity1).value, 2);

    const ECRegistry& constRegistry = registry;
    ASSERT_TRUE(constRegistry.Has<CompA>(entity0));
    ASSERT_EQ(constRegistry.Find<CompA>(entity0)->value, 1);
    ASSERT_EQ(constRegistry.Find<CompA>(entity1)->value, 2);
    ASSERT_EQ(constRegistry.Get<CompA>(entity0).value, 1);
    ASSERT_EQ(constRegistry.Get<CompA>(entity1).value, 2);

    registry.Remove<CompA>(entity0);
    ASSERT_FALSE(constRegistry.Has<CompA>(entity0));

    const auto entity2 = registry.Create();
    registry.Emplace<CompA>(entity2, 3);
    registry.Emplace<CompB>(entity2, 4.0f);
    ASSERT_TRUE(constRegistry.Has<CompA>(entity2));
    ASSERT_TRUE(constRegistry.Has<CompB>(entity2));
    ASSERT_EQ(constRegistry.Get<CompA>(entity2).value, 3);
    ASSERT_EQ(constRegistry.Get<CompB>(entity2).value, 4.0f);

    const auto entity3 = registry.Create();
    registry.Emplace<CompA>(entity3, 5);
    registry.Emplace<CompB>(entity3, 6.0f);
    registry.Remove<CompA>(entity3);
    ASSERT_FALSE(constRegistry.Has<CompA>(entity3));
    ASSERT_TRUE(constRegistry.Has<CompB>(entity3));
    ASSERT_EQ(constRegistry.Get<CompB>(entity3).value, 6.0f);

    std::unordered_set expected = { entity1, entity2 };
    const auto view0 = registry.View<CompA>();
    ASSERT_EQ(view0.Size(), 2);
    view0.Each([&](Entity e, CompA& compA) -> void {
        ASSERT_TRUE(expected.contains(e));
    });

    expected = { entity1 };
    const auto view1 = registry.ConstView<CompA>(Exclude<CompB> {});
    ASSERT_EQ(view1.Size(), 1);
    view1.Each([&](Entity e, const CompA& compA) -> void {
        ASSERT_TRUE(expected.contains(e));
    });

    expected = { entity2, entity3 };
    const auto view2 = registry.View<CompB>();
    ASSERT_EQ(view2.Size(), 2);
    for (const auto& [entity, compB] : view2) {
        ASSERT_TRUE(expected.contains(entity));
    }

    expected = { entity3 };
    const auto view3 = registry.ConstView<CompB>(Exclude<CompA> {});
    ASSERT_EQ(view3.Size(), 1);
    for (const auto& [entity, compB] : view3) {
        ASSERT_TRUE(expected.contains(entity));
    }
}

TEST(ECSTest, ComponentDynamicTest)
{
    CompClass compAClass = &Mirror::Class::Get<CompA>();
    CompClass compBClass = &Mirror::Class::Get<CompB>();

    ECRegistry registry;
    const ECRegistry& constRegistry = registry;
    const auto entity0 = registry.Create();
    const auto entity1 = registry.Create();
    registry.EmplaceDyn(compAClass, entity0, Mirror::ForwardAsArgList(1));
    registry.EmplaceDyn(compAClass, entity1, Mirror::ForwardAsArgList(2));
    ASSERT_TRUE(registry.HasDyn(compAClass, entity0));
    ASSERT_TRUE(constRegistry.HasDyn(compAClass, entity1));
    ASSERT_EQ(registry.GetDyn(compAClass, entity0).As<CompA&>().value, 1);
    ASSERT_EQ(constRegistry.GetDyn(compAClass, entity1).As<const CompA&>().value, 2);
    ASSERT_EQ(registry.FindDyn(compAClass, entity0).As<CompA&>().value, 1);
    ASSERT_EQ(constRegistry.FindDyn(compAClass, entity1).As<const CompA&>().value, 2);

    std::unordered_set expected = { entity0, entity1 };
    const auto view0 = registry.ConstRuntimeView(
        RuntimeFilter()
            .IncludeDyn(compAClass));
    ASSERT_EQ(view0.Size(), 2);
    view0.Each([&](Entity e, const CompA& compA) -> void {
        ASSERT_TRUE(expected.contains(e));
    });

    const auto entity2 = registry.Create();
    registry.EmplaceDyn(compAClass, entity2, Mirror::ForwardAsArgList(3));
    registry.EmplaceDyn(compBClass, entity2, Mirror::ForwardAsArgList(4.0f));
    const auto entity3 = registry.Create();
    registry.EmplaceDyn(compBClass, entity3, Mirror::ForwardAsArgList(5.0f));

    const auto view1 = registry.RuntimeView(
        RuntimeFilter()
            .Include<CompA>());
    expected = { entity0, entity1, entity2 };
    ASSERT_EQ(view1.Size(), 3);
    for (const auto& entity : view1) {
        ASSERT_TRUE(expected.contains(entity));
    }

    const auto view2 = registry.ConstRuntimeView(
        RuntimeFilter()
            .Include<CompA>()
            .Exclude<CompB>());
    expected = { entity0, entity1 };
    ASSERT_EQ(view2.Size(), 2);
    view2.Each([&](Entity e, const CompA& compA) -> void {
        ASSERT_TRUE(expected.contains(e));
    });

    const auto view3 = registry.RuntimeView(
        RuntimeFilter()
            .Include<CompB>());
    expected = { entity2, entity3 };
    ASSERT_EQ(view3.Size(), 2);
    view3.Each([&](Entity e, const CompB& compB) -> void {
        ASSERT_TRUE(expected.contains(e));
    });

    const auto view4 = registry.ConstRuntimeView(
        RuntimeFilter()
            .Include<CompB>()
            .Exclude<CompA>());
    expected = { entity3 };
    ASSERT_EQ(view4.Size(), 1);
    view4.Each([&](Entity e, const CompB& compB) -> void {
        ASSERT_TRUE(expected.contains(e));
    });

    const auto view5 = constRegistry.ConstRuntimeView(
        RuntimeFilter()
            .Include<CompA>()
            .Include<CompB>());
    expected = { entity2 };
    ASSERT_EQ(view5.Size(), 1);
    view5.Each([&](Entity e, const CompA& compA, const CompB& compB) -> void {
        ASSERT_TRUE(expected.contains(e));
    });
}

TEST(ECSTest, GlobalComponentStaticTest)
{
    ECRegistry registry;
    registry.GEmplace<GCompA>(1);
    ASSERT_TRUE(registry.GHas<GCompA>());
    ASSERT_EQ(registry.GFind<GCompA>()->value, 1);
    ASSERT_EQ(registry.GGet<GCompA>().value, 1);

    const ECRegistry& constRegistry = registry;
    ASSERT_TRUE(constRegistry.GHas<GCompA>());
    ASSERT_EQ(constRegistry.GFind<GCompA>()->value, 1);
    ASSERT_EQ(constRegistry.GGet<GCompA>().value, 1);

    registry.GEmplace<GCompB>(2.0f);
    ASSERT_TRUE(registry.GHas<GCompA>());
    ASSERT_TRUE(registry.GHas<GCompB>());
    ASSERT_EQ(registry.GGet<GCompB>().value, 2.0f);

    registry.GRemove<GCompB>();
    ASSERT_TRUE(registry.GHas<GCompA>());
    ASSERT_FALSE(registry.GHas<GCompB>());
}

TEST(ECSTest, GlobalComponentDynamicTest)
{
    GCompClass gCompAClass = &Mirror::Class::Get<GCompA>();
    GCompClass gCompBClass = &Mirror::Class::Get<GCompB>();

    ECRegistry registry;
    registry.GEmplaceDyn(gCompAClass, Mirror::ForwardAsArgList(1));
    ASSERT_TRUE(registry.GHasDyn(gCompAClass));
    ASSERT_EQ(registry.GFindDyn(gCompAClass).As<GCompA&>().value, 1);
    ASSERT_EQ(registry.GGetDyn(gCompAClass).As<GCompA&>().value, 1);

    const ECRegistry& constRegistry = registry;
    ASSERT_TRUE(constRegistry.GHasDyn(gCompAClass));
    ASSERT_EQ(constRegistry.GFindDyn(gCompAClass).As<const GCompA&>().value, 1);
    ASSERT_EQ(constRegistry.GGetDyn(gCompAClass).As<const GCompA&>().value, 1);

    registry.GEmplaceDyn(gCompBClass, Mirror::ForwardAsArgList(2.0f));
    ASSERT_TRUE(registry.GHasDyn(gCompAClass));
    ASSERT_TRUE(registry.GHasDyn(gCompBClass));
    ASSERT_EQ(registry.GGetDyn(gCompBClass).As<const GCompB&>().value, 2.0f);

    registry.GRemoveDyn(gCompBClass);
    ASSERT_TRUE(registry.GHasDyn(gCompAClass));
    ASSERT_FALSE(registry.GHasDyn(gCompBClass));
}

TEST(ECSTest, ComponentEventStaticTest)
{
    EventCounts count;
    ECRegistry registry;
    registry.Events<CompA>().onConstructed.BindLambda([&](ECRegistry&, Entity) -> void { count.onConstructed++; });
    registry.Events<CompA>().onUpdated.BindLambda([&](ECRegistry&, Entity) -> void { count.onUpdated++; });
    registry.Events<CompA>().onRemove.BindLambda([&](ECRegistry&, Entity) -> void { count.onRemove++; });

    const auto entity0 = registry.Create();
    const auto entity1 = registry.Create();
    registry.Emplace<CompA>(entity0, 1);
    ASSERT_EQ(count, EventCounts(1, 0, 0));
    registry.Emplace<CompA>(entity1, 2);
    ASSERT_EQ(count, EventCounts(2, 0, 0));

    {
        const auto updater = registry.Update<CompA>(entity0);
        updater->value = 3;
    }
    ASSERT_EQ(count, EventCounts(2, 1, 0));
    registry.Update<CompA>(entity0, [](CompA& compA) -> void {
        compA.value = 4;
    });
    ASSERT_EQ(count, EventCounts(2, 2, 0));
    registry.Get<CompA>(entity1).value = 5;
    registry.NotifyUpdated<CompA>(entity1);
    ASSERT_EQ(count, EventCounts(2, 3, 0));

    registry.Remove<CompA>(entity0);
    ASSERT_EQ(count, EventCounts(2, 3, 1));
    registry.Remove<CompA>(entity1);
    ASSERT_EQ(count, EventCounts(2, 3, 2));
}

TEST(ECSTest, ComponentEventDynamicTest)
{
    CompClass compAClass = &Mirror::Class::Get<CompA>();
    EventCounts count;
    ECRegistry registry;
    registry.EventsDyn(compAClass).onConstructed.BindLambda([&](ECRegistry&, Entity) -> void { count.onConstructed++; });
    registry.EventsDyn(compAClass).onUpdated.BindLambda([&](ECRegistry&, Entity) -> void { count.onUpdated++; });
    registry.EventsDyn(compAClass).onRemove.BindLambda([&](ECRegistry&, Entity) -> void { count.onRemove++; });

    const auto entity0 = registry.Create();
    const auto entity1 = registry.Create();
    registry.EmplaceDyn(compAClass, entity0, Mirror::ForwardAsArgList(1));
    ASSERT_EQ(count, EventCounts(1, 0, 0));
    registry.EmplaceDyn(compAClass, entity1, Mirror::ForwardAsArgList(2));
    ASSERT_EQ(count, EventCounts(2, 0, 0));

    {
        const auto updater = registry.UpdateDyn(compAClass, entity0);
        updater.As<CompA&>().value = 3;
    }
    ASSERT_EQ(count, EventCounts(2, 1, 0));
    registry.UpdateDyn(compAClass, entity0, [](const Mirror::Any& ref) -> void {
        ref.As<CompA&>().value = 4;
    });
    ASSERT_EQ(count, EventCounts(2, 2, 0));
    registry.GetDyn(compAClass, entity1).As<CompA&>().value = 5;
    registry.NotifyUpdatedDyn(compAClass, entity1);
    ASSERT_EQ(count, EventCounts(2, 3, 0));

    registry.RemoveDyn(compAClass, entity0);
    ASSERT_EQ(count, EventCounts(2, 3, 1));
    registry.RemoveDyn(compAClass, entity1);
    ASSERT_EQ(count, EventCounts(2, 3, 2));
}

TEST(ECSTest, GlobalComponentEventStaticTest)
{
    EventCounts count;
    ECRegistry registry;
    registry.GEvents<GCompA>().onConstructed.BindLambda([&](ECRegistry&) -> void { count.onConstructed++; });
    registry.GEvents<GCompA>().onUpdated.BindLambda([&](ECRegistry&) -> void { count.onUpdated++; });
    registry.GEvents<GCompA>().onRemove.BindLambda([&](ECRegistry&) -> void { count.onRemove++; });

    registry.GEmplace<GCompA>(1);
    ASSERT_EQ(count, EventCounts(1, 0, 0));

    {
        const auto updater = registry.GUpdate<GCompA>();
        updater->value = 3;
    }
    ASSERT_EQ(count, EventCounts(1, 1, 0));
    registry.GUpdate<GCompA>([](GCompA& gCompA) -> void {
        gCompA.value = 4;
    });
    ASSERT_EQ(count, EventCounts(1, 2, 0));
    registry.GGet<GCompA>().value = 5;
    registry.GNotifyUpdated<GCompA>();
    ASSERT_EQ(count, EventCounts(1, 3, 0));

    registry.GRemove<GCompA>();
    ASSERT_EQ(count, EventCounts(1, 3, 1));
}

TEST(ECSTest, GlobalComponentEventDynamicTest)
{
    GCompClass gCompAClass = &Mirror::Class::Get<GCompA>();
    EventCounts count;
    ECRegistry registry;
    registry.GEventsDyn(gCompAClass).onConstructed.BindLambda([&](ECRegistry&) -> void { count.onConstructed++; });
    registry.GEventsDyn(gCompAClass).onUpdated.BindLambda([&](ECRegistry&) -> void { count.onUpdated++; });
    registry.GEventsDyn(gCompAClass).onRemove.BindLambda([&](ECRegistry&) -> void { count.onRemove++; });

    registry.GEmplaceDyn(gCompAClass, Mirror::ForwardAsArgList(1));
    ASSERT_EQ(count, EventCounts(1, 0, 0));

    {
        const auto updater = registry.GUpdateDyn(gCompAClass);
        updater.As<GCompA&>().value = 3;
    }
    ASSERT_EQ(count, EventCounts(1, 1, 0));
    registry.GUpdateDyn(gCompAClass, [](const Mirror::Any& ref) -> void {
        ref.As<GCompA&>().value = 4;
    });
    ASSERT_EQ(count, EventCounts(1, 2, 0));
    registry.GGetDyn(gCompAClass).As<GCompA&>().value = 5;
    registry.GNotifyUpdatedDyn(gCompAClass);
    ASSERT_EQ(count, EventCounts(1, 3, 0));

    registry.GRemove<GCompA>();
    ASSERT_EQ(count, EventCounts(1, 3, 1));
}

TEST(ECSTest, ObserverTest)
{
    ECRegistry registry;
    auto observer = registry.Observer();
    observer
        .ObConstructed<CompA>()
        .ObUpdatedDyn(&Mirror::Class::Get<CompB>());

    const auto entity0 = registry.Create();
    ASSERT_EQ(observer.Size(), 0);

    registry.Emplace<CompA>(entity0, 1);
    ASSERT_EQ(observer.Size(), 1);
    std::unordered_set expected = { entity0 };
    observer.Each([&](Entity e) -> void {
        ASSERT_TRUE(expected.contains(e));
    });

    const auto entity1 = registry.Create();
    registry.Emplace<CompB>(entity1, 2.0f);
    ASSERT_EQ(observer.Size(), 1);
    registry.Update<CompB>(entity1, [](CompB& compB) -> void {
        compB.value = 3.0f;
    });
    ASSERT_EQ(observer.Size(), 2);
    expected = { entity0, entity1 };
    for (const auto e : expected) {
        ASSERT_TRUE(expected.contains(e));
    }
}

TEST(ECSTest, ECRegistryCopyTest)
{
    ECRegistry registry0;
    const auto entity0 = registry0.Create();
    const auto entity1 = registry0.Create();
    registry0.Emplace<CompA>(entity0, 1);
    registry0.Emplace<CompB>(entity1, 2.0f);

    ECRegistry registry1 = registry0;
    ASSERT_EQ(registry1.Get<CompA>(entity0).value, 1);
    ASSERT_EQ(registry1.Get<CompB>(entity1).value, 2.0f);
}
