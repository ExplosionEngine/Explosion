//
// Created by johnk on 2024/3/31.
//

#include <QApplication>
#include <Editor/Core.h>
#include <Editor/QmlHotReload.h>
#include <Editor/Widget/Launcher.h>

int main(int argc, char* argv[])
{
    Editor::Core::Get().Initialize(argc, argv);

    QApplication qtApplication(argc, argv);

    Common::UniqueRef<QWidget> mainWindow;
    if (!Editor::Core::Get().ProjectHasSet()) {
        mainWindow = new Editor::Launcher();
    } else {
        // TODO editor main
    }
    mainWindow->show();

    auto& qmlHotReloadEngine = Editor::QmlHotReloadEngine::Get();
    qmlHotReloadEngine.Start();
    const int execRes = QApplication::exec();
    qmlHotReloadEngine.Stop();

    mainWindow = nullptr;
    Editor::Core::Get().Cleanup();
    return execRes;
}
