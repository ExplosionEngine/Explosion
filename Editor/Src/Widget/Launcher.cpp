//
// Created by johnk on 2024/6/23.
//

#include <Editor/Widget/Launcher.h>
#include <Editor/Widget/moc_Launcher.cpp> // NOLINT

namespace Editor {
    Launcher::Launcher()
        : QmlWidget("Launcher.qml")
    {
        setFixedSize(1024, 768);
    }
}
