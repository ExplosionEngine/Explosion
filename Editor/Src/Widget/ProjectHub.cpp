//
// Created by johnk on 2024/6/23.
//

#include <Editor/Widget/ProjectHub.h>
#include <Editor/Widget/moc_ProjectHub.cpp> // NOLINT

namespace Editor {
    ProjectHub::ProjectHub()
        : QmlWidget("EProjectHub.qml")
    {
        setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
        setFixedSize(800, 600);
    }
}
