//
// Created by johnk on 2024/3/31.
//

#include <QApplication>

#include <Core/Cmdline.h>
#include <Editor/Core.h>
#include <Editor/QmlEngine.h>
#include <Editor/Widget/Launcher.h>
#include <Editor/Widget/WidgetSamples.h>

#if BUILD_CONFIG_DEBUG
static ::Core::CmdlineArgValue<bool> caRunSample(
    "widgetSamples", "-widgetSamples", false,
    "Whether to run widget samples instead of editor");
#endif

int main(int argc, char* argv[])
{
    Editor::Core::Get().Initialize(argc, argv);

    QApplication qtApplication(argc, argv);

    auto& qmlEngine = Editor::QmlEngine::Get();
    qmlEngine.Start();

    Common::UniquePtr<QWidget> mainWidget;
#if BUILD_CONFIG_DEBUG
    if (caRunSample.GetValue()) {
        mainWidget = new Editor::WidgetSamples();
    } else
#endif
    if (!Editor::Core::Get().ProjectHasSet()) { // NOLINT
        mainWidget = new Editor::Launcher();
    } else {
        // TODO editor main
    }
    mainWidget->show();

    const int execRes = QApplication::exec();
    qmlEngine.Stop();

    mainWidget = nullptr;
    Editor::Core::Get().Cleanup();
    return execRes;
}
