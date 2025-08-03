//
// Created by johnk on 2025/8/3.
//

#pragma once

#include <QWebEngineView>

namespace Editor {
    class ProjectHub final : public QWebEngineView {
        Q_OBJECT

    public:
        explicit ProjectHub(QWidget* inParent = nullptr);
    };
}
