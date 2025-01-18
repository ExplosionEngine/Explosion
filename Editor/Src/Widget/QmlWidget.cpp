//
// Created by Kindem on 2024/12/31.
//

#include <QVBoxLayout>

#include <Editor/Widget/QmlWidget.h>
#include <Editor/Widget/moc_QmlWidget.cpp> // NOLINT

namespace Editor {
    QmlWidget::QmlWidget(const std::string& qmlFileName, QWidget* parent)
        : QWidget(parent)
        , url(QUrl(QString(std::format("qrc:/qt/qml/editor/{}", qmlFileName).c_str())))
        , quickView(new QQuickView)
    {
        quickView->setResizeMode(QQuickView::SizeRootObjectToView);
        quickView->setSource(url);
        quickView->show();

        QWidget* quickViewContainer = createWindowContainer(quickView);
        quickViewContainer->setMinimumSize(quickView->size());
        quickViewContainer->setFocusPolicy(Qt::TabFocus);

        QVBoxLayout* layout = new QVBoxLayout(); // NOLINT
        setLayout(layout);

        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(quickViewContainer);

        // TODO support hot reload
    }

    QQuickView* QmlWidget::GetQuickView() // NOLINT
    {
        return quickView;
    }

    const QUrl& QmlWidget::GetQmlUrl() const
    {
        return url;
    }
} // namespace Editor
