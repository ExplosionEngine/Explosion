//
// Created by johnk on 2024/3/31.
//

#include <QApplication>
#include <Editor/Core.h>
#include <Editor/Widget/ProjectSelector.h>

int main(int argc, char* argv[])
{
    Editor::Core::Get().Initialize(argc, argv);

    QApplication qtApplication(argc, argv);

    Common::UniqueRef<QWidget> mainWindow;
    if (!Editor::Core::Get().ProjectHasSet()) {
        mainWindow = new Editor::QProjectSelector();
    } else {
        // TODO editor main
    }
    mainWindow->show();
    const int execRes = QApplication::exec();

    mainWindow = nullptr;
    Editor::Core::Get().Cleanup();
    return execRes;
}
