//
// Created by Kindem on 2024/12/31.
//

#pragma once

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <filesystem>

#include <QQuickView>
#include <QFileSystemWatcher>

namespace Editor {
    class QmlHotReloadEngine {
    public:
        static QmlHotReloadEngine& Get();

        ~QmlHotReloadEngine();

        QUrl GetUrlFromShort(const std::string& inQmlShortFileName) const;
        void Start();
        void Stop();
        void Register(const std::string& inQmlShotFileName, QQuickView* inView);
        void Unregister(const std::string& inQmlShotFileName, QQuickView* inView);

    private:
        QmlHotReloadEngine();

        std::filesystem::path qmlSourceDirectory;
        std::filesystem::path qmlModuleDirectory;
        std::unordered_map<std::string, std::unordered_set<QQuickView*>> liveQuickViewsMap;
        Common::UniqueRef<QFileSystemWatcher> watcher;
    };
}
