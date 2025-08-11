//
// Created by johnk on 2025/8/3.
//

#pragma once

#include <Editor/Widget/WebWidget.h>

namespace Editor {
    class ProjectHub;

    class ProjectHubBridge final : public QObject {
        Q_OBJECT

    public:
        explicit ProjectHubBridge(ProjectHub* parent = nullptr);

    public Q_SLOTS:
        void CreateProject() const;
    };

    class ProjectHub final : public WebWidget {
        Q_OBJECT

    public:
        explicit ProjectHub(QWidget* inParent = nullptr);

    private:
        ProjectHubBridge* bridge;
    };
}
