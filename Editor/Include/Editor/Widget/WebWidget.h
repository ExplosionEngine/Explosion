//
// Created by johnk on 2025/8/9.
//

#pragma once

#include <QWebChannel>
#include <QWebEngineView>

namespace Editor {
    class WebWidget : public QWebEngineView {
        Q_OBJECT

    public:
        explicit WebWidget(QWidget* inParent = nullptr);
        ~WebWidget() override;

        void Load(const std::string& inUrl);

    protected:
        QWebChannel* GetWebChannel() const;

    private:
        QWebChannel* webChannel;
    };
}
