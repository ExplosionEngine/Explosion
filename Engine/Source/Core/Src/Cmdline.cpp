//
// Created by johnk on 2023/7/25.
//

#include <sstream>

#include <Core/Cmdline.h>
#include <Common/Debug.h>

namespace Core {
    CmdlineArg::~CmdlineArg() = default;

    Cli& Cli::Get()
    {
        static Cli instance;
        return instance;
    }

    Cli::Cli()
        : parsed(false)
    {
    }

    Cli::~Cli() = default;

    std::pair<bool, std::string> Cli::Parse(const int argc, char* argv[])
    {
        Assert(!parsed);
        parsed = true;

        clipp::group cli;
        for (auto* arg : args) {
            cli.push_back(arg->CreateClippParameter());
        }

        if (!clipp::parse(argc, argv, cli)) {
            std::stringstream stream;
            stream << clipp::make_man_page(cli, argv[0]);
            return std::make_pair(false, stream.str());
        }
        return std::make_pair(true, "");
    }
}
