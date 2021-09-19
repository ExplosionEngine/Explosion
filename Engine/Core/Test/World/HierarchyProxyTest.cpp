//
// Created by Zach Lee on 2021/9/19.
//

#include <gtest/gtest.h>

#include <Engine/ECS.h>
#include <Engine/Render/Systems/HierarchyProxy.h>
using namespace Explosion;

TEST(HierarchyProxyTest, SetParent01)
{
    using namespace ECS;
    Registry registry;

    auto e1 = registry.CreateEntity();
    auto e2 = registry.CreateEntity();
    auto e3 = registry.CreateEntity();

    auto proxy = registry.CreateProxy<HierarchyProxy>();
    proxy.SetParent(e2, e1);
    proxy.SetParent(e3, e1);

    ASSERT_EQ(proxy.GetParent(e1), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetFirstChild(e1), e2);
    ASSERT_EQ(proxy.GetPrevSibling(e1), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetNextSibling(e1), INVALID_ENTITY);

    {
        auto children = proxy.GetChildren(e1);
        ASSERT_EQ(children.size(), (size_t)2);
        ASSERT_EQ(children[0], e2);
        ASSERT_EQ(children[1], e3);
    }

    ASSERT_EQ(proxy.GetParent(e2), e1);
    ASSERT_EQ(proxy.GetFirstChild(e2), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetPrevSibling(e2), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetNextSibling(e2), e3);

    {
        auto children = proxy.GetChildren(e2);
        ASSERT_EQ(children.size(), (size_t)0);
    }

    ASSERT_EQ(proxy.GetParent(e3), e1);
    ASSERT_EQ(proxy.GetFirstChild(e3), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetPrevSibling(e3), e2);
    ASSERT_EQ(proxy.GetNextSibling(e3), INVALID_ENTITY);

    {
        auto children = proxy.GetChildren(e3);
        ASSERT_EQ(children.size(), (size_t)0);
    }
}

TEST(HierarchyProxyTest, SetParent02)
{
    using namespace ECS;
    Registry registry;

    auto e1 = registry.CreateEntity();
    auto e2 = registry.CreateEntity();
    auto e3 = registry.CreateEntity();

    auto proxy = registry.CreateProxy<HierarchyProxy>();
    proxy.SetParent(e2, e1);
    proxy.SetParent(e3, e2);

    ASSERT_EQ(proxy.GetParent(e1), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetFirstChild(e1), e2);
    ASSERT_EQ(proxy.GetPrevSibling(e1), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetNextSibling(e1), INVALID_ENTITY);

    {
        auto children = proxy.GetChildren(e1);
        ASSERT_EQ(children.size(), (size_t)1);
        ASSERT_EQ(children[0], e2);
    }

    ASSERT_EQ(proxy.GetParent(e2), e1);
    ASSERT_EQ(proxy.GetFirstChild(e2), e3);
    ASSERT_EQ(proxy.GetPrevSibling(e2), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetNextSibling(e2), INVALID_ENTITY);

    {
        auto children = proxy.GetChildren(e2);
        ASSERT_EQ(children.size(), (size_t)1);
        ASSERT_EQ(children[0], e3);
    }

    ASSERT_EQ(proxy.GetParent(e3), e2);
    ASSERT_EQ(proxy.GetFirstChild(e3), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetPrevSibling(e3), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetNextSibling(e3), INVALID_ENTITY);

    {
        auto children = proxy.GetChildren(e3);
        ASSERT_EQ(children.size(), (size_t)0);
    }
}

TEST(HierarchyProxyTest, SetParent03)
{
    using namespace ECS;
    Registry registry;

    auto e1 = registry.CreateEntity();
    auto e2 = registry.CreateEntity();
    auto e3 = registry.CreateEntity();
    auto e4 = registry.CreateEntity();

    auto proxy = registry.CreateProxy<HierarchyProxy>();
    proxy.SetParent(e2, e1);
    proxy.SetParent(e3, e1);
    proxy.SetParent(e4, e3);
    proxy.SetParent(e3, e2);

    ASSERT_EQ(proxy.GetParent(e1), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetFirstChild(e1), e2);
    ASSERT_EQ(proxy.GetPrevSibling(e1), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetNextSibling(e1), INVALID_ENTITY);

    {
        auto children = proxy.GetChildren(e1);
        ASSERT_EQ(children.size(), (size_t)1);
        ASSERT_EQ(children[0], e2);
    }

    ASSERT_EQ(proxy.GetParent(e2), e1);
    ASSERT_EQ(proxy.GetFirstChild(e2), e3);
    ASSERT_EQ(proxy.GetPrevSibling(e2), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetNextSibling(e2), INVALID_ENTITY);

    {
        auto children = proxy.GetChildren(e2);
        ASSERT_EQ(children.size(), (size_t)1);
        ASSERT_EQ(children[0], e3);
    }

    ASSERT_EQ(proxy.GetParent(e3), e2);
    ASSERT_EQ(proxy.GetFirstChild(e3), e4);
    ASSERT_EQ(proxy.GetPrevSibling(e3), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetNextSibling(e3), INVALID_ENTITY);

    {
        auto children = proxy.GetChildren(e3);
        ASSERT_EQ(children.size(), (size_t)1);
        ASSERT_EQ(children[0], e4);
    }

    ASSERT_EQ(proxy.GetParent(e4), e3);
    ASSERT_EQ(proxy.GetFirstChild(e4), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetPrevSibling(e4), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetNextSibling(e4), INVALID_ENTITY);

    {
        auto children = proxy.GetChildren(e4);
        ASSERT_EQ(children.size(), (size_t)0);
    }
}

TEST(HierarchyProxyTest, SetParent04)
{
    using namespace ECS;
    Registry registry;

    auto e1 = registry.CreateEntity();
    auto e2 = registry.CreateEntity();
    auto e3 = registry.CreateEntity();
    auto e4 = registry.CreateEntity();

    auto proxy = registry.CreateProxy<HierarchyProxy>();
    proxy.SetParent(e2, e1);
    proxy.SetParent(e3, e1);
    proxy.SetParent(e4, e2);
    proxy.SetParent(e2, e3);


    ASSERT_EQ(proxy.GetParent(e1), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetFirstChild(e1), e3);
    ASSERT_EQ(proxy.GetPrevSibling(e1), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetNextSibling(e1), INVALID_ENTITY);

    {
        auto children = proxy.GetChildren(e1);
        ASSERT_EQ(children.size(), (size_t)1);
        ASSERT_EQ(children[0], e3);
    }

    ASSERT_EQ(proxy.GetParent(e2), e3);
    ASSERT_EQ(proxy.GetFirstChild(e2), e4);
    ASSERT_EQ(proxy.GetPrevSibling(e2), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetNextSibling(e2), INVALID_ENTITY);

    {
        auto children = proxy.GetChildren(e2);
        ASSERT_EQ(children.size(), (size_t)1);
        ASSERT_EQ(children[0], e4);
    }

    ASSERT_EQ(proxy.GetParent(e3), e1);
    ASSERT_EQ(proxy.GetFirstChild(e3), e2);
    ASSERT_EQ(proxy.GetPrevSibling(e3), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetNextSibling(e3), INVALID_ENTITY);

    {
        auto children = proxy.GetChildren(e3);
        ASSERT_EQ(children.size(), (size_t)1);
        ASSERT_EQ(children[0], e2);
    }

    ASSERT_EQ(proxy.GetParent(e4), e2);
    ASSERT_EQ(proxy.GetFirstChild(e4), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetPrevSibling(e4), INVALID_ENTITY);
    ASSERT_EQ(proxy.GetNextSibling(e4), INVALID_ENTITY);

    {
        auto children = proxy.GetChildren(e4);
        ASSERT_EQ(children.size(), (size_t)0);
    }
}