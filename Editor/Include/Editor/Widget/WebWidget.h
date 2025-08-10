//
// Created by johnk on 2025/8/9.
//

#pragma once

#include <QWebEngineView>

namespace Editor {
    class WebWidget : public QWebEngineView {
        Q_OBJECT

    public:
        explicit WebWidget(QWidget* inParent = nullptr);
        ~WebWidget() override;

        void Load(const std::string& inUrl);
    };
}
