//
// Created by johnk on 2025/3/25.
//

#pragma once

#include <Common/Memory.h>
#include <Runtime/Engine.h>

namespace Runtime {
    template <typename T>
    class RenderThreadPtr {
    public:
        RenderThreadPtr();
        RenderThreadPtr(T* inPtr); // NOLINT
        RenderThreadPtr(Common::SharedPtr<T> inPtr); // NOLINT
        RenderThreadPtr(Common::UniquePtr<T>&& inPtr); // NOLINT
        ~RenderThreadPtr();

        DefaultCopyable(RenderThreadPtr) // NOLINT
        DefaultMovable(RenderThreadPtr) // NOLINT

        T* operator->() const noexcept;
        T& operator*() const noexcept;
        bool operator==(nullptr_t) const noexcept;
        bool operator!=(nullptr_t) const noexcept;
        explicit operator bool() const noexcept;

        bool Valid() const;
        T* Get() const;
        void Reset(T* pointer = nullptr);

    private:
        Common::SharedPtr<T> ptr;
    };
}

namespace Runtime {
    template <typename T>
    RenderThreadPtr<T>::RenderThreadPtr() = default;

    template <typename T>
    RenderThreadPtr<T>::RenderThreadPtr(T* inPtr)
        : ptr(inPtr)
    {
    }

    template <typename T>
    RenderThreadPtr<T>::RenderThreadPtr(Common::SharedPtr<T> inPtr)
        : ptr(std::move(inPtr))
    {
    }

    template <typename T>
    RenderThreadPtr<T>::RenderThreadPtr(Common::UniquePtr<T>&& inPtr)
        : ptr(std::move(inPtr))
    {
    }

    template <typename T>
    RenderThreadPtr<T>::~RenderThreadPtr()
    {
        EngineHolder::Get().GetRenderModule().GetRenderThread().EmplaceTask([transferPtr = std::move(ptr)]() mutable -> void {
            transferPtr.Reset();
        });
    }

    template <typename T>
    T* RenderThreadPtr<T>::operator->() const noexcept
    {
        Assert(Core::ThreadContext::IsGameThread());
        return ptr.Get();
    }

    template <typename T>
    T& RenderThreadPtr<T>::operator*() const noexcept
    {
        Assert(Core::ThreadContext::IsGameThread());
        return *ptr;
    }

    template <typename T>
    bool RenderThreadPtr<T>::operator==(nullptr_t) const noexcept
    {
        return ptr == nullptr;
    }

    template <typename T>
    bool RenderThreadPtr<T>::operator!=(nullptr_t) const noexcept
    {
        return ptr != nullptr;
    }

    template <typename T>
    RenderThreadPtr<T>::operator bool() const noexcept
    {
        return Valid();
    }

    template <typename T>
    bool RenderThreadPtr<T>::Valid() const
    {
        return ptr != nullptr;
    }

    template <typename T>
    T* RenderThreadPtr<T>::Get() const
    {
        Assert(Core::ThreadContext::IsGameThread());
        return ptr.Get();
    }

    template <typename T>
    void RenderThreadPtr<T>::Reset(T* pointer)
    {
        ptr.Reset(pointer);
    }
}
