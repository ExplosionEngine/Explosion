//
// Created by johnk on 2024/3/31.
//

#include <QApplication>

#include <Core/Cmdline.h>
#include <Editor/QmlEngine.h>
#include <Editor/Widget/ProjectHub.h>
#include <Editor/Widget/WidgetSamples.h>

#if BUILD_CONFIG_DEBUG
static ::Core::CmdlineArgValue<bool> caRunSample(
    "widgetSamples", "-widgetSamples", false,
    "Whether to run widget samples instead of editor");
#endif

static ::Core::CmdlineArgValue<std::string> caRhiType(
    "rhiType", "-rhi", RHI::GetPlatformDefaultRHIAbbrString(),
    "rhi abbr string, can be 'dx12' or 'vulkan'");

static ::Core::CmdlineArgValue<std::string> caProjectRoot(
    "projectRoot", "-project", "",
    "project root path");

static void InitializePreQtApp(int argc, char** argv)
{
    Core::Cli::Get().Parse(argc, argv);

    Runtime::EngineInitParams params {};
    params.logToFile = true;
    params.gameRoot = caProjectRoot.GetValue();
    params.rhiType = caRhiType.GetValue();

    Runtime::EngineHolder::Load("Editor", params);
}

static void InitializePostQtApp()
{
    Editor::QmlEngine::Get().Start();
}

static void Cleanup()
{
    Editor::QmlEngine::Get().Stop();
    Runtime::EngineHolder::Unload();
}

int main(int argc, char* argv[])
{
    InitializePreQtApp(argc, argv);
    QApplication qtApplication(argc, argv);
    InitializePostQtApp();

    Common::UniquePtr<QWidget> mainWidget;
#if BUILD_CONFIG_DEBUG
    if (caRunSample.GetValue()) {
        mainWidget = new Editor::WidgetSamples();
    } else
#endif
    if (caProjectRoot.GetValue().empty()) { // NOLINT
        mainWidget = new Editor::ProjectHub();
    } else {
        // TODO editor main
    }
    mainWidget->show();

    const int execRes = QApplication::exec();
    mainWidget = nullptr;
    Cleanup();
    return execRes;
}
