//
// Created by johnk on 2024/3/31.
//

#include <Editor/Application.h>
#include <Core/Cmdline.h>
#include <RHI/RHI.h>

namespace Editor {
    static Core::CmdlineArgValue<std::string> caRhiType(
        "RHIType",
        "-rhi",
        RHI::GetPlatformDefaultRHIAbbrString(),
        "RHI type editor running with."
    );

    static Core::CmdlineArgValue<std::string> caProject(
        "Project",
        "-project",
        "",
        "Project opened after editor startup."
    );

    Application& Application::Get()
    {
        static Application instance;
        return instance;
    }

    Application::Application() = default;

    Application::~Application() = default;

    void Application::SetUp(int argc, char* argv[])
    {
        SetupCli(argc, argv);
    }

    void Application::SetupCli(int argc, char* argv[])
    {
        Core::Cli::Get().Parse(argc, argv);
    }

    void Application::SetupRHI()
    {
        auto rhiAbbrString = caRhiType.GetValue();
        RHI::RHIType rhiType = RHI::RHIAbbrStringToRHIType(rhiAbbrString);

        // TODO
    }

    void Application::SetupProject()
    {
        // TODO setup project
    }
}
