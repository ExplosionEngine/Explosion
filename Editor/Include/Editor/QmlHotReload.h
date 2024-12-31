//
// Created by Kindem on 2024/12/31.
//

#pragma once

#include <unordered_set>

#include <QQuickView>

namespace Editor {
    class QmlHotReloadEngine {
    public:
        static QmlHotReloadEngine& Get();

        ~QmlHotReloadEngine();

        void Start();
        void Stop();
        void Register(QQuickView* inView);
        void Unregister(QQuickView* inView);

    private:
        QmlHotReloadEngine();

        std::unordered_set<QQuickView*> liveQuickViews;
    };
}
