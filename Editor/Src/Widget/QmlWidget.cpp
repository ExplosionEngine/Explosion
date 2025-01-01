//
// Created by Kindem on 2024/12/31.
//

#include <QVBoxLayout>

#include <Editor/Widget/QmlWidget.h>
#include <Editor/Widget/moc_QmlWidget.cpp> // NOLINT

namespace Editor {
    QmlWidget::QmlWidget(std::string inShortQmlFileName, QWidget* inParent)
        : QWidget(inParent)
        , shortQmlFileName(std::move(inShortQmlFileName))
        , quickView(new QQuickView)
    {
        quickView->setResizeMode(QQuickView::SizeRootObjectToView);
        quickView->setSource(QmlHotReloadEngine::Get().GetUrlFromShort(shortQmlFileName));

        quickViewContainer = createWindowContainer(quickView);
        quickViewContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        quickViewContainer->show();

        QVBoxLayout* layout = new QVBoxLayout(); // NOLINT
        setLayout(layout);

        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(quickViewContainer);

        QmlHotReloadEngine::Get().Register(shortQmlFileName, quickView);
    }

    QmlWidget::~QmlWidget()
    {
        QmlHotReloadEngine::Get().Unregister(shortQmlFileName, quickView);
    }

    const std::string& QmlWidget::GetShotQmlFileName() const
    {
        return shortQmlFileName;
    }

    QQuickView* QmlWidget::GetQuickView() // NOLINT
    {
        return quickView;
    }

    QWidget* QmlWidget::GetQuickViewContainer() const
    {
        return quickViewContainer;
    }
} // namespace Editor
