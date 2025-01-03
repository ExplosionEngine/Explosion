//
// Created by Kindem on 2024/12/31.
//

#pragma once

#include <unordered_set>
#include <unordered_map>
#include <string>

#include <QQuickView>
#include <QFileSystemWatcher>

#include <Core/Paths.h>

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

        Common::Path qmlSourceDirectory;
        Common::Path qmlModuleDirectory;
        std::unordered_map<std::string, std::unordered_set<QQuickView*>> liveQuickViewsMap;
        Common::UniqueRef<QFileSystemWatcher> watcher;
    };
}
