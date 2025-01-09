//
// Created by Kindem on 2024/12/31.
//

#pragma once

#include <unordered_set>

#include <QFileSystemWatcher>

#include <Core/Paths.h>
#include <Editor/Widget/QmlWidget.h>

namespace Editor {
    class QmlEngine {
    public:
        static QmlEngine& Get();

        ~QmlEngine();

        QUrl GetUrlFromShort(const std::string& inQmlShortFileName) const;
        void Start();
        void Stop();
        void Register(QmlWidget* inWidget);
        void Unregister(QmlWidget* inWidget);

    private:
        QmlEngine();

        void ReloadSingletonTypes() const;

        Common::Path qmlSourceDir;
        std::unordered_set<QmlWidget*> widgets;
        Common::UniqueRef<QFileSystemWatcher> watcher;
    };
}
