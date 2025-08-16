//
// Created by johnk on 2024/3/31.
//

#include <QApplication>
#include <QNetworkProxy>

#include <Core/Cmdline.h>
#include <Runtime/Engine.h>
#include <Editor/Widget/Editor.h>
#include <Editor/Widget/ProjectHub.h>
#include <Editor/WebUIServer.h>

#if BUILD_CONFIG_DEBUG
#include <Editor/Widget/GraphicsSampleWidget.h>

static Core::CmdlineArgValue<bool> caGraphicsSample(
    "graphicsSample", "-graphicsSample", false,
    "Whether to run graphics sample instead of editor");
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
    Editor::WebUIServer::Get().Start();

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
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
}

static void Cleanup(EditorApplicationModel inModel)
{
    Editor::WebUIServer::Get().Stop();

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
#endif
    if (inModel == EditorApplicationModel::projectHub) { // NOLINT
        return new Editor::ProjectHub();
    }
    return new Editor::Editor();
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
