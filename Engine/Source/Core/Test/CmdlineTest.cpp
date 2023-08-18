//
// Created by johnk on 2023/7/25.
//

#include <gtest/gtest.h>

#include <Core/Cmdline.h>

Core::CmdlineArgValue<bool> arg0("a", "-a", false, "arg0");
Core::CmdlineArgValue<int32_t> arg1("b", "-b", 0, "arg1");
Core::CmdlineArgValue<std::string> arg2("c", "-c", "", "arg2");

bool arg3 = false;
uint32_t arg4 = 0;
std::string arg5;
Core::CmdlineArgRef<bool> arg3Ref("d", "-d", arg3, "arg3");
Core::CmdlineArgRef<uint32_t> arg4Ref("e", "-e", arg4, "arg4");
Core::CmdlineArgRef<std::string> arg5Ref("f", "-f", arg5, "arg5");

TEST(CmdlineTest, BasicTest)
{
    std::vector<char*> args = {
        const_cast<char*>("TestApplication"),
        const_cast<char*>("-a"),
        const_cast<char*>("-b"),
        const_cast<char*>("1"),
        const_cast<char*>("-c"),
        const_cast<char*>("hello"),
        const_cast<char*>("-d"),
        const_cast<char*>("-e"),
        const_cast<char*>("2"),
        const_cast<char*>("-f"),
        const_cast<char*>("world"),
    };

    auto parseResult = Core::Cli::Get().Parse(static_cast<int>(args.size()), args.data());
    ASSERT_TRUE(parseResult.first);
    ASSERT_TRUE(arg0.GetValue());
    ASSERT_EQ(arg1.GetValue(), 1);
    ASSERT_EQ(arg2.GetValue(), "hello");
    ASSERT_TRUE(arg3);
    ASSERT_EQ(arg4, 2);
    ASSERT_EQ(arg5, "world");
}
