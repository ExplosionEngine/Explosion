//
// Created by johnk on 2024/3/31.
//

#include <QApplication>
#include <QPushButton>

#include <Editor/Application.h>

int main(int argc, char* argv[])
{
    QApplication qtApplication(argc, argv);
    Editor::Application::Get().SetUp(argc, argv);
    return QApplication::exec();
}
