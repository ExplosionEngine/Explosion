//
// Created by Kindem on 2024/12/31.
//

#include <QQmlContext>
#include <QVBoxLayout>

#include <Editor/Widget/QmlWidget.h>
#include <Editor/QmlEngine.h>
#include <Editor/Widget/moc_QmlWidget.cpp> // NOLINT

namespace Editor {
    QmlWidget::QmlWidget(std::string inShortQmlFileName, QWidget* inParent)
        : QWidget(inParent)
        , shortQmlFileName(std::move(inShortQmlFileName))
        , url(QmlEngine::Get().GetUrlFromShort(shortQmlFileName))
        , quickView(new QQuickView)
    {

        const Common::Path resDir = Core::Paths::EngineResDir();
        quickView->rootContext()->setContextProperty("appResDir", QString::fromStdString(resDir.String()));
        quickView->setResizeMode(QQuickView::SizeRootObjectToView);
        quickView->setSource(url);

        quickViewContainer = createWindowContainer(quickView);
        quickViewContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        quickViewContainer->show();

        QVBoxLayout* layout = new QVBoxLayout(); // NOLINT
        setLayout(layout);

        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(quickViewContainer);

        QmlEngine::Get().Register(this);
    }

    QmlWidget::~QmlWidget()
    {
        QmlEngine::Get().Unregister(this);
    }

    const std::string& QmlWidget::GetShotQmlFileName() const
    {
        return shortQmlFileName;
    }

    const QUrl& QmlWidget::GetUrl() const
    {
        return url;
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
