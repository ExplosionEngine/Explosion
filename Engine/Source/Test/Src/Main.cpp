//
// Created by johnk on 2022/9/5.
//

#include <Test/Test.h>
#include <Core/Cmdline.h>

int main(int argc, char* argv[])
{
    Core::Cli::Get().Parse(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
