//
// Created by johnk on 2023/4/13.
//

#pragma once

#include <memory>

#include <Common/Utility.h>

namespace Common {
    template <typename T>
    class UniqueRef {
    public:
        NON_COPYABLE(UniqueRef)
        UniqueRef(T* pointer) : ref(pointer) {} // NOLINT
        UniqueRef(std::unique_ptr<T>&& inRef) : ref(std::move(inRef)) {} // NOLINT
        UniqueRef(UniqueRef&& other) noexcept : ref(std::move(other.ref)) {} // NOLINT
        UniqueRef() = default;
        ~UniqueRef() = default;

        UniqueRef& operator=(T* pointer)
        {
            ref = std::unique_ptr<T>(pointer);
            return *this;
        }

        UniqueRef& operator=(std::unique_ptr<T>&& inRef)
        {
            ref = std::move(inRef);
            return *this;
        }

        UniqueRef& operator=(UniqueRef&& other) noexcept
        {
            ref = std::move(other.ref);
            return *this;
        }

        T* operator->() const noexcept
        {
            return ref.operator->();
        }

        T& operator*() const noexcept
        {
            return ref.operator*();
        }

        bool operator==(nullptr_t) const noexcept
        {
            return ref == nullptr;
        }

        bool operator!=(nullptr_t) const noexcept
        {
            return ref != nullptr;
        }

        T* Get() const
        {
            return ref.get();
        }

        void Reset(T* pointer = nullptr)
        {
            ref.reset(pointer);
        }

    private:
        std::unique_ptr<T> ref;
    };

    template <typename T>
    class SharedRef {
    public:
        template <typename T2> SharedRef(std::shared_ptr<T2>& inRef) : ref(inRef) {} // NOLINT
        template <typename T2> SharedRef(std::shared_ptr<T2>&& inRef) : ref(std::move(inRef)) {} // NOLINT
        SharedRef(T* pointer) : ref(pointer) {} // NOLINT
        SharedRef(SharedRef& other) : ref(other.ref) {} // NOLINT
        SharedRef(SharedRef&& other) noexcept : ref(std::move(other.ref)) {} // NOLINT
        SharedRef() = default;
        ~SharedRef() = default;

        template <typename T2>
        SharedRef& operator=(std::shared_ptr<T2>& inRef)
        {
            ref = inRef;
            return *this;
        }

        template <typename T2>
        SharedRef& operator=(std::shared_ptr<T2>&& inRef)
        {
            ref = std::move(inRef);
            return *this;
        }

        SharedRef& operator=(T* pointer)
        {
            ref = std::shared_ptr<T>(pointer);
            return *this;
        }

        SharedRef& operator=(SharedRef& other) // NOLINT
        {
            ref = other.ref;
            return *this;
        }

        SharedRef& operator=(SharedRef&& other) noexcept
        {
            ref = std::move(other.ref);
            return *this;
        }

        T* operator->() const noexcept
        {
            return ref.operator->();
        }

        T& operator*() const noexcept
        {
            return ref.operator*();
        }

        bool operator==(nullptr_t) const noexcept
        {
            return ref == nullptr;
        }

        bool operator!=(nullptr_t) const noexcept
        {
            return ref != nullptr;
        }

        T* Get() const
        {
            return ref.get();
        }

        void Reset(T* pointer = nullptr)
        {
            ref.reset(pointer);
        }

        auto RefCount() const
        {
            return ref.use_count();
        }

        template <typename T2>
        SharedRef<T2> StaticCast()
        {
            return static_pointer_cast<T2, T>(ref);
        }

        template <typename T2>
        SharedRef<T2> DynamicCast()
        {
            return dynamic_pointer_cast<T2, T>(ref);
        }

        template <typename T2>
        SharedRef<T2> ReinterpretCast()
        {
            return reinterpret_pointer_cast<T2, T>(ref);
        }

        std::shared_ptr<T>& GetStd()
        {
            return ref;
        }

    private:
        std::shared_ptr<T> ref;
    };

    template <typename T>
    class WeakRef {
    public:
        template <typename T2> WeakRef(SharedRef<T2>& inRef) : ref(inRef.GetStd()) {} // NOLINT
        WeakRef(WeakRef& other) : ref(other.ref) {} // NOLINT
        WeakRef(WeakRef&& other) noexcept : ref(std::move(other.ref)) {} // NOLINT

        template <typename T2>
        WeakRef& operator=(SharedRef<T2>& inRef)
        {
            ref = inRef.GetStd();
            return *this;
        }

        WeakRef& operator=(WeakRef& other) // NOLINT
        {
            ref = other.ref;
            return *this;
        }

        WeakRef& operator=(WeakRef&& other) noexcept
        {
            ref = std::move(other.ref);
            return *this;
        }

        void Reset()
        {
            ref.reset();
        }

        bool Expired() const
        {
            return ref.expired();
        }

        SharedRef<T> Lock() const
        {
            return ref.lock();
        }

    private:
        std::weak_ptr<T> ref;
    };

    template <typename T, typename... Args>
    Common::UniqueRef<T> MakeUnique(Args&&... args)
    {
        return Common::UniqueRef<T>(new T(std::forward<Args>(args)...));
    }

    template <typename T, typename... Args>
    Common::SharedRef<T> MakeShared(Args&&... args)
    {
        return Common::SharedRef<T>(new T(std::forward<Args>(args)...));
    }
}
