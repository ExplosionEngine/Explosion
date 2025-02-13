//
// Created by johnk on 2023/10/16.
//

#include <Test/Test.h>

#include <AssetTest.h>

TEST(AssetTest, AssetRefTest0)
{
    AssetPtr<TestAsset> a0 = MakeShared<TestAsset>();
    ASSERT_EQ(a0.RefCount(), 1);

    AssetPtr<TestAsset> a1 = a0;
    ASSERT_EQ(a0.RefCount(), 2);
    ASSERT_EQ(a1.RefCount(), 2);

    AssetPtr<Asset> a2 = a1.StaticCast<Asset>();
    ASSERT_EQ(a0.RefCount(), 3);
    ASSERT_EQ(a1.RefCount(), 3);
    ASSERT_EQ(a2.RefCount(), 3);
}

TEST(AssetTest, AssetRefTest1)
{
    AssetPtr<TestAsset> a0 = MakeShared<TestAsset>();
    AssetPtr<TestAsset> a1 = std::move(a0);
    ASSERT_EQ(a1.RefCount(), 1);

    WeakAssetPtr<TestAsset> w0 = a1;
    ASSERT_FALSE(w0.Expired());

    AssetPtr<Asset> a2 = a1.StaticCast<Asset>();
    WeakAssetPtr<Asset> w1 = a2;
    ASSERT_FALSE(w1.Expired());

    a1.Reset();
    a2.Reset();
    ASSERT_TRUE(w0.Expired());
    ASSERT_TRUE(w1.Expired());
}

TEST(AssetTest, SaveLoadTest)
{
    static Core::Uri uri("asset://Engine/Test/Generated/Runtime/AssetTest.SaveLoadTest");

    AssetPtr<TestAsset> asset = MakeShared<TestAsset>(uri, 1, "hello");
    AssetManager::Get().Save(asset);

    AssetPtr<TestAsset> restore = AssetManager::Get().SyncLoad<TestAsset>(uri);
    ASSERT_EQ(restore.Uri(), uri);
    ASSERT_EQ(restore->a, 1);
    ASSERT_EQ(restore->b, "hello");
}

TEST(AssetTest, AsyncLoadTest)
{
    static Core::Uri uri("asset://Engine/Test/Generated/Runtime/AssetTest.SaveLoadTest");

    AssetPtr<TestAsset> asset = MakeShared<TestAsset>(uri, 1, "hello");
    AssetManager::Get().Save(asset);

    AssetManager::Get().AsyncLoad<TestAsset>(uri, [&](AssetPtr<TestAsset> restore) -> void {
        ASSERT_EQ(restore.Uri(), uri);
        ASSERT_EQ(restore->a, 1);
        ASSERT_EQ(restore->b, "hello");
    });
}
