//
// Created by johnk on 2024/3/31.
//

#include <QApplication>

#include <Core/Cmdline.h>
#include <Editor/QmlEngine.h>
#include <Editor/Widget/ProjectHub.h>
#include <Editor/Widget/WidgetSamples.h>

#if BUILD_CONFIG_DEBUG
#include <Editor/Widget/GraphicsSampleWidget.h>

static Core::CmdlineArgValue<bool> caGraphicsSample(
    "graphicsSample", "-graphicsSample", false,
    "Whether to run graphics sample instead of editor");

static Core::CmdlineArgValue<bool> caWidgetSamples(
    "widgetSamples", "-widgetSamples", false,
    "Whether to run widget samples instead of editor");
#endif

static Core::CmdlineArgValue<std::string> caRhiType(
    "rhiType", "-rhi", RHI::GetPlatformDefaultRHIAbbrString(),
    "rhi abbr string, can be 'dx12' or 'vulkan'");

static Core::CmdlineArgValue<std::string> caProjectRoot(
    "projectRoot", "-project", "",
    "project root path");

enum class EditorApplicationModel : uint8_t {
#if BUILD_CONFIG_DEBUG
    graphicsSample,
    widgetSamples,
#endif
    projectHub,
    editor,
    max
};

static EditorApplicationModel GetAppModel()
{
#if BUILD_CONFIG_DEBUG
    if (caGraphicsSample.GetValue()) {
        return EditorApplicationModel::graphicsSample;
    }
    if (caWidgetSamples.GetValue()) {
        return EditorApplicationModel::widgetSamples;
    }
#endif
    if (caProjectRoot.GetValue().empty()) {
        return EditorApplicationModel::projectHub;
    }
    return EditorApplicationModel::editor;
}

static bool NeedInitCore(EditorApplicationModel inModel)
{
    bool result = inModel == EditorApplicationModel::editor;
#if BUILD_CONFIG_DEBUG
    result = result || inModel == EditorApplicationModel::graphicsSample;
#endif
    return result;
}

static void InitializePreQtApp(EditorApplicationModel inModel)
{
    if (!NeedInitCore(inModel)) {
        return;
    }

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

static void Cleanup(EditorApplicationModel inModel)
{
    Editor::QmlEngine::Get().Stop();

    if (!NeedInitCore(inModel)) {
        return;
    }
    Runtime::EngineHolder::Unload();
}

static Common::UniquePtr<QWidget> CreateMainWidget(EditorApplicationModel inModel)
{
#if BUILD_CONFIG_DEBUG
    if (inModel == EditorApplicationModel::graphicsSample) {
        return new Editor::GraphicsSampleWidget();
    }
    if (inModel == EditorApplicationModel::widgetSamples) {
        return new Editor::WidgetSamples();
    }
#endif
    if (inModel == EditorApplicationModel::projectHub) { // NOLINT
        return new Editor::ProjectHub();
    }
    // TODO replace with editor main widget
    return nullptr;
}

int main(int argc, char* argv[])
{
    Core::Cli::Get().Parse(argc, argv);
    const auto appModel = GetAppModel();

    InitializePreQtApp(appModel);
    QApplication qtApplication(argc, argv);
    InitializePostQtApp();

    Common::UniquePtr<QWidget> mainWidget = CreateMainWidget(appModel);
    mainWidget->show();

    const int execRes = QApplication::exec();
    mainWidget.Reset();
    Cleanup(appModel);
    return execRes;
}
