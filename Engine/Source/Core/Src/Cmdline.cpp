//
// Created by johnk on 2023/7/25.
//

#include <sstream>

#include <Core/Cmdline.h>
#include <Core/Paths.h>
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

    std::pair<bool, std::string> Cli::Parse(int argc, char* argv[], bool force)
    {
        if (!force) {
            Assert(!parsed);
        }
        parsed = true;

        Core::Paths::SetExecutableDir(argv[0]);

        clipp::group cli;
        for (const auto& [name, arg] : args) {
            cli.push_back(arg->CreateClippParameter());
        }

        if (!clipp::parse(argc, argv, cli)) {
            std::stringstream stream;
            stream << clipp::make_man_page(cli, argv[0]);
            return std::make_pair(false, stream.str());
        }
        return std::make_pair(true, "");
    }

    CmdlineArg* Cli::FindArg(const std::string& name) const
    {
        const auto iter = args.find(name);
        return iter == args.end() ? nullptr : iter->second;
    }

    CmdlineArg& Cli::GetArg(const std::string& name) const
    {
        auto* result = FindArg(name);
        Assert(result != nullptr);
        return *result;
    }
}
