//
// Created by johnk on 2023/4/13.
//

#pragma once

#include <memory>
#include <mutex>

#include <Common/Utility.h>

namespace Common {
    template <typename T>
    class UniqueRef {
    public:
        NonCopyable(UniqueRef)
        UniqueRef(T* pointer); // NOLINT
        UniqueRef(std::unique_ptr<T>&& inRef); // NOLINT
        UniqueRef(UniqueRef&& other) noexcept; // NOLINT
        UniqueRef();
        ~UniqueRef();

        UniqueRef& operator=(T* pointer);
        UniqueRef& operator=(std::unique_ptr<T>&& inRef);
        UniqueRef& operator=(UniqueRef&& other) noexcept;

        T* operator->() const noexcept;
        T& operator*() const noexcept;
        bool operator==(nullptr_t) const noexcept;
        bool operator!=(nullptr_t) const noexcept;

        T* Get() const;
        void Reset(T* pointer = nullptr);

    private:
        std::unique_ptr<T> ref;
    };

    template <typename T>
    class SharedRef {
    public:
        template <typename T2> SharedRef(std::shared_ptr<T2>& inRef); // NOLINT
        template <typename T2> SharedRef(std::shared_ptr<T2>&& inRef) noexcept; // NOLINT
        SharedRef(T* pointer); // NOLINT
        SharedRef(const SharedRef& other); // NOLINT
        SharedRef(SharedRef&& other) noexcept; // NOLINT
        SharedRef();
        ~SharedRef();

        template <typename T2>
        SharedRef& operator=(std::shared_ptr<T2>& inRef);

        template <typename T2>
        SharedRef& operator=(std::shared_ptr<T2>&& inRef);

        SharedRef& operator=(T* pointer);
        SharedRef& operator=(SharedRef& other); // NOLINT
        SharedRef& operator=(SharedRef&& other) noexcept;
        T* operator->() const noexcept;
        T& operator*() const noexcept;
        bool operator==(nullptr_t) const noexcept;
        bool operator!=(nullptr_t) const noexcept;

        T* Get() const;
        void Reset(T* pointer = nullptr);
        auto RefCount() const;
        std::shared_ptr<T>& GetStd();

        template <typename T2>
        SharedRef<T2> StaticCast();

        template <typename T2>
        SharedRef<T2> DynamicCast();

        template <typename T2>
        SharedRef<T2> ReinterpretCast();

    private:
        std::shared_ptr<T> ref;
    };

    template <typename T>
    class WeakRef {
    public:
        template <typename T2> WeakRef(SharedRef<T2>& inRef); // NOLINT
        WeakRef(WeakRef& other); // NOLINT
        WeakRef(WeakRef&& other) noexcept; // NOLINT
        ~WeakRef();

        template <typename T2>
        WeakRef& operator=(SharedRef<T2>& inRef);

        WeakRef& operator=(WeakRef& other); // NOLINT
        WeakRef& operator=(WeakRef&& other) noexcept;

        void Reset();
        bool Expired() const;
        SharedRef<T> Lock() const;

    private:
        std::weak_ptr<T> ref;
    };

    template <typename T, typename... Args>
    Common::UniqueRef<T> MakeUnique(Args&&... args);

    template <typename T, typename... Args>
    Common::SharedRef<T> MakeShared(Args&&... args);
}

namespace Common {
    template <typename T>
    UniqueRef<T>::UniqueRef(T* pointer)
        : ref(pointer)
    {
    }

    template <typename T>
    UniqueRef<T>::UniqueRef(std::unique_ptr<T>&& inRef)
        : ref(std::move(inRef))
    {
    }

    template <typename T>
    UniqueRef<T>::UniqueRef(UniqueRef&& other) noexcept
        : ref(std::move(other.ref))
    {
    }

    template <typename T>
    UniqueRef<T>::UniqueRef() = default;

    template <typename T>
    UniqueRef<T>::~UniqueRef() = default;

    template <typename T>
    UniqueRef<T>& UniqueRef<T>::operator=(T* pointer)
    {
        ref = std::unique_ptr<T>(pointer);
        return *this;
    }

    template <typename T>
    UniqueRef<T>& UniqueRef<T>::operator=(std::unique_ptr<T>&& inRef)
    {
        ref = std::move(inRef);
        return *this;
    }

    template <typename T>
    UniqueRef<T>& UniqueRef<T>::operator=(UniqueRef&& other) noexcept
    {
        ref = std::move(other.ref);
        return *this;
    }

    template <typename T>
    T* UniqueRef<T>::operator->() const noexcept
    {
        return ref.operator->();
    }

    template <typename T>
    T& UniqueRef<T>::operator*() const noexcept
    {
        return ref.operator*();
    }

    template <typename T>
    bool UniqueRef<T>::operator==(nullptr_t) const noexcept
    {
        return ref == nullptr;
    }

    template <typename T>
    bool UniqueRef<T>::operator!=(nullptr_t) const noexcept
    {
        return ref != nullptr;
    }

    template <typename T>
    T* UniqueRef<T>::Get() const
    {
        return ref.get();
    }

    template <typename T>
    void UniqueRef<T>::Reset(T* pointer)
    {
        ref.reset(pointer);
    }

    template <typename T>
    template <typename T2>
    SharedRef<T>::SharedRef(std::shared_ptr<T2>& inRef)
        : ref(inRef)
    {
    }

    template <typename T>
    template <typename T2>
    SharedRef<T>::SharedRef(std::shared_ptr<T2>&& inRef) noexcept
        : ref(std::move(inRef))
    {
    }

    template <typename T>
    SharedRef<T>::SharedRef(T* pointer)
        : ref(pointer)
    {
    }

    template <typename T>
    SharedRef<T>::SharedRef(const SharedRef& other)
        : ref(other.ref)
    {
    }

    template <typename T>
    SharedRef<T>::SharedRef(SharedRef && other) noexcept
        : ref(std::move(other.ref))
    {
    }

    template <typename T>
    SharedRef<T>::SharedRef() = default;

    template <typename T>
    SharedRef<T>::~SharedRef() = default;

    template <typename T>
    template <typename T2>
    SharedRef<T> & SharedRef<T>::operator=(std::shared_ptr<T2> & inRef)
    {
        ref = inRef;
        return *this;
    }

    template <typename T>
    template <typename T2>
    SharedRef<T> & SharedRef<T>::operator=(std::shared_ptr<T2> && inRef)
    {
        ref = std::move(inRef);
        return *this;
    }

    template <typename T>
    SharedRef<T> & SharedRef<T>::operator=(T* pointer)
    {
        ref = std::shared_ptr<T>(pointer);
        return *this;
    }

    template <typename T>
    SharedRef<T> & SharedRef<T>::operator=(SharedRef & other)
    {
        ref = other.ref;
        return *this;
    }

    template <typename T>
    SharedRef<T> & SharedRef<T>::operator=(SharedRef && other) noexcept
    {
        ref = std::move(other.ref);
        return *this;
    }

    template <typename T>
    T* SharedRef<T>::operator->() const noexcept
    {
        return ref.operator->();
    }

    template <typename T>
    T& SharedRef<T>::operator*() const noexcept
    {
        return ref.operator*();
    }

    template <typename T>
    bool SharedRef<T>::operator==(nullptr_t) const noexcept
    {
        return ref == nullptr;
    }

    template <typename T>
    bool SharedRef<T>::operator!=(nullptr_t) const noexcept
    {
        return ref != nullptr;
    }

    template <typename T>
    T* SharedRef<T>::Get() const
    {
        return ref.get();
    }

    template <typename T>
    void SharedRef<T>::Reset(T* pointer)
    {
        ref.reset(pointer);
    }

    template <typename T>
    auto SharedRef<T>::RefCount() const
    {
        return ref.use_count();
    }

    template <typename T>
    std::shared_ptr<T> & SharedRef<T>::GetStd()
    {
        return ref;
    }

    template <typename T>
    template <typename T2>
    SharedRef<T2> SharedRef<T>::StaticCast()
    {
        return static_pointer_cast<T2, T>(ref);
    }

    template <typename T>
    template <typename T2>
    SharedRef<T2> SharedRef<T>::DynamicCast()
    {
        return dynamic_pointer_cast<T2, T>(ref);
    }

    template <typename T>
    template <typename T2>
    SharedRef<T2> SharedRef<T>::ReinterpretCast()
    {
        return reinterpret_pointer_cast<T2, T>(ref);
    }

    template <typename T>
    template <typename T2>
    WeakRef<T>::WeakRef(SharedRef<T2> & inRef)
        : ref(inRef.GetStd())
    {
    }

    template <typename T>
    WeakRef<T>::WeakRef(WeakRef& other)
        : ref(other.ref)
    {
    }

    template <typename T>
    WeakRef<T>::WeakRef(WeakRef && other) noexcept
        : ref(std::move(other.ref))
    {
    }

    template <typename T>
    WeakRef<T>::~WeakRef() = default;

    template <typename T>
    template <typename T2>
    WeakRef<T>& WeakRef<T>::operator=(SharedRef<T2> & inRef)
    {
        ref = inRef.GetStd();
        return *this;
    }

    template <typename T>
    WeakRef<T>& WeakRef<T>::operator=(WeakRef & other)
    {
        ref = other.ref;
        return *this;
    }

    template <typename T>
    WeakRef<T> & WeakRef<T>::operator=(WeakRef && other) noexcept
    {
        ref = std::move(other.ref);
        return *this;
    }

    template <typename T>
    void WeakRef<T>::Reset()
    {
        ref.reset();
    }

    template <typename T>
    bool WeakRef<T>::Expired() const
    {
        return ref.expired();
    }

    template <typename T>
    SharedRef<T> WeakRef<T>::Lock() const
    {
        return ref.lock();
    }

    template <typename T, typename... Args>
    UniqueRef<T> MakeUnique(Args && ... args)
    {
        return Common::UniqueRef<T>(new T(std::forward<Args>(args)...));
    }

    template <typename T, typename... Args>
    SharedRef<T> MakeShared(Args && ... args)
    {
        return Common::SharedRef<T>(new T(std::forward<Args>(args)...));
    }
}
