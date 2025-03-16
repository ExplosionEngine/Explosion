//
// Created by johnk on 2025/2/27.
//

#include <string>

#include <Test/Test.h>
#include <Core/Console.h>

static Core::ConsoleSettingValue<int32_t> csA("a", "", 1);
static Core::ConsoleSettingValue<bool> csB("b", "", true);
static Core::ConsoleSettingValue<std::string> csC("c", "", "hello");

TEST(ConsoleTest, ConsoleSettingTest)
{
    Core::ScopedThreadTag tag(Core::ThreadTag::game);

    ASSERT_EQ(csA.Get(), 1);
    ASSERT_TRUE(csB.Get());
    ASSERT_EQ(csC.Get(), "hello");

    auto& console = Core::Console::Get();
    console.GetSetting("a").SetI32(2);
    console.GetSetting("b").SetBool(false);
    console.GetSetting("c").SetI32(1);

    ASSERT_EQ(csA.Get(), 2);
    ASSERT_FALSE(csB.Get());
    ASSERT_EQ(csC.Get(), "1");
}
