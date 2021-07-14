//
// Created by John Kindem on 2021/7/14.
//

#include <gflags/gflags.h>

DEFINE_string(input, "", "input asset file path");
DEFINE_string(meta, "", "meta config file path");
DEFINE_string(output, "", "output resource file path");

int main(int argc, char* argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    // TODO
}
