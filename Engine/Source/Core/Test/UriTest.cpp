//
// Created by johnk on 2025/2/21.
//

#include <Test/Test.h>
#include <Core/Uri.h>

TEST(UriTest, BasicTest)
{
    const Core::Uri u0("file://Engine/Test/a.txt");
    ASSERT_EQ(u0.Str(), "file://Engine/Test/a.txt");
    ASSERT_EQ(u0.Protocol(), Core::UriProtocol::file);
    ASSERT_EQ(u0.Content(), "Engine/Test/a.txt");
}

TEST(UriTest, FileProtocolTest)
{
    const Core::Uri u0("file://Engine/Test/a.txt");
    const Core::FileUriParser p0(u0);
    ASSERT_EQ(p0.Parse(), Core::Paths::EngineRootDir() / "Test" / "a.txt");

    const Core::Uri u1("file://Game/Test/a.txt");
    const Core::FileUriParser p1(u1);
    ASSERT_EQ(p1.Parse(), Core::Paths::GameRootDir() / "Test" / "a.txt");

    const Core::Uri u2("file://a.txt");
    const Core::FileUriParser p2(u2);
    ASSERT_EQ(p2.Parse(), Common::Path("a.txt"));
}

TEST(UriTest, AssetProtocolTest)
{
    Core::Paths::SetGameRoot("/Game");

    const Core::Uri u0("asset://Engine/Map/Main");
    const Core::AssetUriParser p0(u0);
    ASSERT_EQ(p0.Parse(), Core::Paths::EngineAssetDir() / "Map" / "Main.expa");

    const Core::Uri u1("asset://Game/Map/Main");
    const Core::AssetUriParser p1(u1);
    ASSERT_EQ(p1.Parse(), Core::Paths::GameAssetDir() / "Map" / "Main.expa");

    const Core::Uri u2("asset://Engine/Plugin/Particle/Sample");
    const Core::AssetUriParser p2(u2);
    ASSERT_EQ(p2.Parse(), Core::Paths::EnginePluginAssetDir("Particle") / "Sample.expa");

    const Core::Uri u3("asset://Game/Plugin/Store/Bootstrap");
    const Core::AssetUriParser p3(u3);
    ASSERT_EQ(p3.Parse(), Core::Paths::GamePluginAssetDir("Store") / "Bootstrap.expa");
}
