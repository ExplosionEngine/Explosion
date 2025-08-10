//
// Created by johnk on 2025/8/3.
//

#include <Editor/Widget/ProjectHub.h>
#include <Editor/Widget/moc_ProjectHub.cpp>

namespace Editor {
    ProjectHub::ProjectHub(QWidget* inParent)
        : WebWidget(inParent)
    {
        // TODO
        Load("/");
    }
} // namespace Editor
