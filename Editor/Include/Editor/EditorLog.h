//
// Created by johnk on 2026/7/5.
//

#pragma once

#include <deque>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>

#include <Core/Log.h>

namespace Editor {
    // log stream attached to the engine logger that keeps a bounded history and forwards structured entries to
    // listeners (the log tab backend), log calls arrive from arbitrary threads so everything is mutex-guarded and
    // listeners must marshal to their own thread themselves
    class EditorLogStream final : public Core::LogStream {
    public:
        using EntryListener = std::function<void(const Core::LogEntry&)>;
        using ListenerHandle = size_t;

        // attaches the singleton stream to the engine logger on first use
        static EditorLogStream& Get();

        void Write(const Core::LogEntry& inEntry) override;
        void Flush() override;

        std::vector<Core::LogEntry> Snapshot() const;
        ListenerHandle AddListener(EntryListener inListener);
        void RemoveListener(ListenerHandle inHandle);

    private:
        static constexpr size_t maxHistoryEntries = 2000;

        EditorLogStream();

        mutable std::mutex mutex;
        std::deque<Core::LogEntry> history;
        std::unordered_map<ListenerHandle, EntryListener> listeners;
        ListenerHandle nextListenerHandle;
    };
}
