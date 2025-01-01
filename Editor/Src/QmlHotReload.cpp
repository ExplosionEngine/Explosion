//
// Created by Kindem on 2024/12/31.
//

#include <QQmlEngine>

#include <Editor/QmlHotReload.h>
#include <Core/Paths.h>

namespace Editor {
    static ::Core::CmdlineArgValue<bool> caQmlHotReload(
        "qmlHotReload", "-qmlHotReload", false,
        "Whether to enable qml hot reload (for editor development)");
}

namespace Editor {
    QmlHotReloadEngine& QmlHotReloadEngine::Get()
    {
        static QmlHotReloadEngine engine;
        return engine;
    }

    QmlHotReloadEngine::QmlHotReloadEngine()
        : qmlSourceDirectory(::Core::Paths::EngineCMakeSourceDir() / "Editor" / "QML")
        , qmlModuleDirectory(::Core::Paths::EngineCMakeBinaryDir() / "Generated" / "QmlModule")
    {
    }

    QmlHotReloadEngine::~QmlHotReloadEngine() = default;

    QUrl QmlHotReloadEngine::GetUrlFromShort(const std::string& inQmlShortFileName) const
    {
        if (caQmlHotReload.GetValue()) {
            const std::filesystem::path qmlSourcePath = qmlSourceDirectory / inQmlShortFileName;
            return QUrl::fromLocalFile(QString::fromStdString(qmlSourcePath.string()));
        } else { // NOLINT
            const auto urlString = std::format("qrc:/qt/qml/editor/{}", inQmlShortFileName);
            return QString::fromStdString(urlString);
        }
    }

    void QmlHotReloadEngine::Start()
    {
        if (!caQmlHotReload.GetValue()) {
            return;
        }

        watcher = new QFileSystemWatcher();
        QObject::connect(watcher.Get(), &QFileSystemWatcher::fileChanged, [this](const QString& inPath) -> void {
            const std::filesystem::path changedQml = inPath.toStdString();
            const std::filesystem::path qmlShortName = changedQml.filename();

            const auto iter = liveQuickViewsMap.find(qmlShortName.string());
            if (iter == liveQuickViewsMap.end()) {
                return;
            }
            for (auto* liveQuickView : iter->second) {
                QUrl url(QString::fromStdString(changedQml.string()));
                liveQuickView->source().clear();
                liveQuickView->engine()->clearComponentCache();
                liveQuickView->setSource(url);
            }
        });
    }

    void QmlHotReloadEngine::Stop()
    {
        if (watcher == nullptr) {
            return;
        }

        QObject::disconnect(watcher.Get(), nullptr, nullptr, nullptr);
        watcher.Reset();
    }

    void QmlHotReloadEngine::Register(const std::string& inQmlShotFileName, QQuickView* inView)
    {
        if (watcher == nullptr) {
            return;
        }

        if (!liveQuickViewsMap.contains(inQmlShotFileName)) {
            const auto qmlFileFullPath = qmlSourceDirectory / inQmlShotFileName;
            watcher->addPath(qmlFileFullPath.string().c_str());
            liveQuickViewsMap.emplace(inQmlShotFileName, std::unordered_set<QQuickView*> {});
        }
        auto& liveQuickViews = liveQuickViewsMap.at(inQmlShotFileName);
        liveQuickViews.emplace(inView);
    }

    void QmlHotReloadEngine::Unregister(const std::string& inQmlShotFileName, QQuickView* inView)
    {
        if (watcher == nullptr) {
            return;
        }

        auto& liveQuickViews = liveQuickViewsMap.at(inQmlShotFileName);
        liveQuickViews.erase(inView);
        if (liveQuickViews.empty()) {
            const auto qmlFileFullPath = qmlSourceDirectory / inQmlShotFileName;
            watcher->removePath(qmlFileFullPath.string().c_str());
            liveQuickViewsMap.erase(inQmlShotFileName);
        }
    }
} // namespace Editor
