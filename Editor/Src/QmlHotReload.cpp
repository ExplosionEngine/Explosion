//
// Created by Kindem on 2024/12/31.
//

#include <Editor/QmlHotReload.h>

namespace Editor {
    QmlHotReloadEngine& QmlHotReloadEngine::Get()
    {
        static QmlHotReloadEngine engine;
        return engine;
    }

    QmlHotReloadEngine::QmlHotReloadEngine() = default;

    QmlHotReloadEngine::~QmlHotReloadEngine() = default;

    void QmlHotReloadEngine::Start()
    {
        // TODO
    }

    void QmlHotReloadEngine::Stop()
    {
        // TODO
    }

    void QmlHotReloadEngine::Register(QQuickView* inView)
    {
        liveQuickViews.emplace(inView);
    }

    void QmlHotReloadEngine::Unregister(QQuickView* inView)
    {
        liveQuickViews.erase(inView);
    }
} // namespace Editor
