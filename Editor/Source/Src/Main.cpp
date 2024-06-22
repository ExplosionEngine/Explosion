//
// Created by johnk on 2024/3/31.
//

#include <QApplication>
#include <Editor/Core.h>

int main(int argc, char* argv[])
{
    Editor::Core::Get().Initialize(argc, argv);

    QApplication qtApplication(argc, argv);
    const int execRes = QApplication::exec();

    Editor::Core::Get().Cleanup();
    return execRes;
}
