//
// Created by Kindem on 2024/12/31.
//

#pragma once

#include <QQuickView>
#include <QWidget>

namespace Editor {
    class QmlWidget : public QWidget {
        Q_OBJECT

    public:
        explicit QmlWidget(const std::string& qmlFileName, QWidget* parent = nullptr);

        QQuickView* GetQuickView();
        const QUrl& GetQmlUrl() const;

    private:
        QUrl url;
        QQuickView* quickView;
    };
}
