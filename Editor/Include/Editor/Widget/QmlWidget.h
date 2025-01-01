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
        explicit QmlWidget(std::string inShortQmlFileName, QWidget* inParent = nullptr);
        ~QmlWidget() override;

        const std::string& GetShotQmlFileName() const;
        QQuickView* GetQuickView();
        QWidget* GetQuickViewContainer() const;

    private:
        std::string shortQmlFileName;
        QQuickView* quickView;
        QWidget* quickViewContainer;
    };
}
