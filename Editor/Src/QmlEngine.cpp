//
// Created by Kindem on 2024/12/31.
//

#include <QQmlEngine>

#include <Editor/QmlEngine.h>
#include <Core/Paths.h>
#include <Common/FileSystem.h>

namespace Editor {
    static ::Core::CmdlineArgValue<bool> caQmlHotReload(
        "qmlHotReload", "-qmlHotReload", false,
        "Whether to enable qml hot reload (for editor development)");
}

namespace Editor {
    QmlEngine& QmlEngine::Get()
    {
        static QmlEngine engine;
        return engine;
    }

    QmlEngine::QmlEngine()
        : qmlSourceDir(::Core::Paths::EngineCMakeSourceDir() / "Editor" / "QML")
    {
    }

    QmlEngine::~QmlEngine() = default;

    QUrl QmlEngine::GetUrlFromShort(const std::string& inQmlShortFileName) const
    {
        if (caQmlHotReload.GetValue()) {
            const Common::Path qmlSourcePath = qmlSourceDir / inQmlShortFileName;
            return QUrl::fromLocalFile(QString::fromStdString(qmlSourcePath.String()));
        } else { // NOLINT
            const auto urlString = std::format("qrc:/qt/qml/editor/{}", inQmlShortFileName);
            return QString::fromStdString(urlString);
        }
    }

    void QmlEngine::Start()
    {
        ReloadSingletonTypes();
        if (!caQmlHotReload.GetValue()) {
            return;
        }

        watcher = new QFileSystemWatcher();
        watcher->addPath(QString::fromStdString(qmlSourceDir.String()));
        qmlSourceDir.TraverseRecurse([&](const Common::Path& path) -> bool {
            if (path.IsDirectory()) {
                watcher->addPath(QString::fromStdString(path.String()));
            }
            return true;
        });

        QObject::connect(watcher.Get(), &QFileSystemWatcher::directoryChanged, [this](const QString&) -> void {
            ReloadSingletonTypes();

            for (auto* widget : widgets) {
                QQuickView* quickView = widget->GetQuickView();
                quickView->source().clear();
                quickView->engine()->clearComponentCache();
                quickView->setSource(widget->GetUrl());
            }
        });
    }

    void QmlEngine::Stop()
    {
        widgets.clear();
        if (watcher == nullptr) {
            return;
        }

        QObject::disconnect(watcher.Get(), nullptr, nullptr, nullptr);
        watcher.Reset();
    }

    void QmlEngine::Register(QmlWidget* inWidget)
    {
        widgets.emplace(inWidget);
    }

    void QmlEngine::Unregister(QmlWidget* inWidget)
    {
        widgets.erase(inWidget);
    }

    void QmlEngine::ReloadSingletonTypes() const
    {
        const std::vector<std::string> singletonSources = Common::StringUtils::Split(SINGLETON_QML_SOURCES, ";");
        for (const auto& singletonSource : singletonSources) {
            const std::string name = Common::Path(singletonSource).FileNameWithoutExtension();
            const QUrl url = GetUrlFromShort(singletonSource);
            qmlRegisterSingletonType(url, name.c_str(), 1, 0, name.c_str());
        }
    }
} // namespace Editor
