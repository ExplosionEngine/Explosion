//
// Created by johnk on 2025/8/3.
//

#pragma once

#include <Editor/Widget/WebWidget.h>

namespace Editor {
    class ProjectHub final : public WebWidget {
        Q_OBJECT

    public:
        explicit ProjectHub(QWidget* inParent = nullptr);
    };
}
