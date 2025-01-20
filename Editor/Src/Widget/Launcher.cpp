//
// Created by johnk on 2024/6/23.
//

#include <Editor/Widget/Launcher.h>
#include <Editor/Widget/moc_Launcher.cpp> // NOLINT

namespace Editor {
    Launcher::Launcher()
        : QmlWidget("ELauncher.qml")
    {
        setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
        setFixedSize(1024, 768);
    }
}
