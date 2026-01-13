//
// Created by johnk on 2025/8/9.
//

#pragma once

#include <QWebChannel>
#include <QWebEngineView>
#include <QWebEnginePage>

namespace Editor {
    class WebPage : public QWebEnginePage {
        Q_OBJECT

    public:
        explicit WebPage(QWidget* inParent = nullptr);
        ~WebPage() override;

    protected:
        void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& message, int lineNumber, const QString& sourceID) override;
    };

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
