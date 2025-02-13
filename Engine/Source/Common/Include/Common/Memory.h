//
// Created by johnk on 2023/4/13.
//

#pragma once

#include <memory>
#include <mutex>

#include <Common/Utility.h>

namespace Common {
    template <typename T>
    class UniquePtr {
    public:
        NonCopyable(UniquePtr)
        UniquePtr(T* pointer); // NOLINT
        UniquePtr(std::unique_ptr<T>&& uniquePtr); // NOLINT
        UniquePtr(UniquePtr&& other) noexcept; // NOLINT
        UniquePtr();
        ~UniquePtr();

        UniquePtr& operator=(T* pointer);
        UniquePtr& operator=(std::unique_ptr<T>&& uniquePtr);
        UniquePtr& operator=(UniquePtr&& other) noexcept;

        T* operator->() const noexcept;
        T& operator*() const noexcept;
        bool operator==(nullptr_t) const noexcept;
        bool operator!=(nullptr_t) const noexcept;
        explicit operator bool() const noexcept;

        bool Valid() const;
        T* Get() const;
        void Reset(T* pointer = nullptr);
        std::unique_ptr<T>& GetStd();

    private:
        std::unique_ptr<T> ptr;
    };

    template <typename T>
    class SharedPtr {
    public:
        template <typename T2> SharedPtr(std::shared_ptr<T2>& sharedPtr); // NOLINT
        template <typename T2> SharedPtr(std::shared_ptr<T2>&& sharedPtr) noexcept; // NOLINT
        template <typename T2> SharedPtr(std::unique_ptr<T2>&& uniquePtr) noexcept; // NOLINT
        template <typename T2> SharedPtr(UniquePtr<T2>&& uniquePtr) noexcept; // NOLINT

        SharedPtr(T* pointer); // NOLINT
        SharedPtr(const SharedPtr& other); // NOLINT
        SharedPtr(SharedPtr&& other) noexcept; // NOLINT
        SharedPtr();
        ~SharedPtr();

        template <typename T2> SharedPtr& operator=(std::shared_ptr<T2>& sharedPtr);
        template <typename T2> SharedPtr& operator=(std::shared_ptr<T2>&& sharedPtr);
        template <typename T2> SharedPtr& operator=(std::unique_ptr<T2>&& uniquePtr) noexcept;
        template <typename T2> SharedPtr& operator=(UniquePtr<T2>&& uniquePtr) noexcept;

        SharedPtr& operator=(T* pointer);
        SharedPtr& operator=(SharedPtr& other); // NOLINT
        SharedPtr& operator=(SharedPtr&& other) noexcept;
        T* operator->() const noexcept;
        T& operator*() const noexcept;
        bool operator==(nullptr_t) const noexcept;
        bool operator!=(nullptr_t) const noexcept;
        explicit operator bool() const noexcept;

        bool Valid() const;
        T* Get() const;
        void Reset(T* pointer = nullptr);
        auto RefCount() const;
        std::shared_ptr<T>& GetStd();

        template <typename T2> SharedPtr<T2> StaticCast();
        template <typename T2> SharedPtr<T2> DynamicCast();
        template <typename T2> SharedPtr<T2> ReinterpretCast();

    private:
        std::shared_ptr<T> ptr;
    };

    template <typename T>
    class WeakPtr {
    public:
        template <typename T2> WeakPtr(SharedPtr<T2>& sharedPtr); // NOLINT

        WeakPtr(WeakPtr& other); // NOLINT
        WeakPtr(WeakPtr&& other) noexcept; // NOLINT
        ~WeakPtr();

        template <typename T2> WeakPtr& operator=(SharedPtr<T2>& sharedPtr);
        WeakPtr& operator=(WeakPtr& other); // NOLINT
        WeakPtr& operator=(WeakPtr&& other) noexcept;

        void Reset();
        bool Expired() const;
        SharedPtr<T> Lock() const;
        std::weak_ptr<T>& GetStd();

    private:
        std::weak_ptr<T> ptr;
    };

    template <typename T, typename... Args> UniquePtr<T> MakeUnique(Args&&... args);
    template <typename T, typename... Args> SharedPtr<T> MakeShared(Args&&... args);
}

namespace Common {
    template <typename T>
    UniquePtr<T>::UniquePtr(T* pointer)
        : ptr(pointer)
    {
    }

    template <typename T>
    UniquePtr<T>::UniquePtr(std::unique_ptr<T>&& uniquePtr)
        : ptr(std::move(uniquePtr))
    {
    }

    template <typename T>
    UniquePtr<T>::UniquePtr(UniquePtr&& other) noexcept
        : ptr(std::move(other.ptr))
    {
    }

    template <typename T>
    UniquePtr<T>::UniquePtr() = default;

    template <typename T>
    UniquePtr<T>::~UniquePtr() = default;

    template <typename T>
    UniquePtr<T>& UniquePtr<T>::operator=(T* pointer)
    {
        ptr = std::unique_ptr<T>(pointer);
        return *this;
    }

    template <typename T>
    UniquePtr<T>& UniquePtr<T>::operator=(std::unique_ptr<T>&& uniquePtr)
    {
        ptr = std::move(uniquePtr);
        return *this;
    }

    template <typename T>
    UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr&& other) noexcept
    {
        ptr = std::move(other.ptr);
        return *this;
    }

    template <typename T>
    T* UniquePtr<T>::operator->() const noexcept
    {
        return ptr.operator->();
    }

    template <typename T>
    T& UniquePtr<T>::operator*() const noexcept
    {
        return ptr.operator*();
    }

    template <typename T>
    bool UniquePtr<T>::operator==(nullptr_t) const noexcept
    {
        return ptr == nullptr;
    }

    template <typename T>
    bool UniquePtr<T>::operator!=(nullptr_t) const noexcept
    {
        return ptr != nullptr;
    }

    template <typename T>
    UniquePtr<T>::operator bool() const noexcept
    {
        return Valid();
    }

    template <typename T>
    T* UniquePtr<T>::Get() const
    {
        return ptr.get();
    }

    template <typename T>
    bool UniquePtr<T>::Valid() const
    {
        return ptr != nullptr;
    }

    template <typename T>
    void UniquePtr<T>::Reset(T* pointer)
    {
        ptr.reset(pointer);
    }

    template <typename T>
    std::unique_ptr<T>& UniquePtr<T>::GetStd()
    {
        return ptr;
    }

    template <typename T>
    template <typename T2>
    SharedPtr<T>::SharedPtr(std::shared_ptr<T2>& sharedPtr)
        : ptr(sharedPtr)
    {
    }

    template <typename T>
    template <typename T2>
    SharedPtr<T>::SharedPtr(std::shared_ptr<T2>&& sharedPtr) noexcept
        : ptr(std::move(sharedPtr))
    {
    }

    template <typename T>
    template <typename T2>
    SharedPtr<T>::SharedPtr(std::unique_ptr<T2>&& uniquePtr) noexcept
        : ptr(std::move(uniquePtr))
    {
    }

    template <typename T>
    template <typename T2>
    SharedPtr<T>::SharedPtr(UniquePtr<T2>&& uniquePtr) noexcept
        : ptr(std::move(uniquePtr.Get()))
    {
    }

    template <typename T>
    SharedPtr<T>::SharedPtr(T* pointer)
        : ptr(pointer)
    {
    }

    template <typename T>
    SharedPtr<T>::SharedPtr(const SharedPtr& other)
        : ptr(other.ptr)
    {
    }

    template <typename T>
    SharedPtr<T>::SharedPtr(SharedPtr && other) noexcept
        : ptr(std::move(other.ptr))
    {
    }

    template <typename T>
    SharedPtr<T>::SharedPtr() = default;

    template <typename T>
    SharedPtr<T>::~SharedPtr() = default;

    template <typename T>
    template <typename T2>
    SharedPtr<T> & SharedPtr<T>::operator=(std::shared_ptr<T2> & sharedPtr)
    {
        ptr = sharedPtr;
        return *this;
    }

    template <typename T>
    template <typename T2>
    SharedPtr<T> & SharedPtr<T>::operator=(std::shared_ptr<T2> && sharedPtr)
    {
        ptr = std::move(sharedPtr);
        return *this;
    }

    template <typename T>
    template <typename T2>
    SharedPtr<T>& SharedPtr<T>::operator=(std::unique_ptr<T2>&& uniquePtr) noexcept
    {
        ptr = std::move(uniquePtr);
        return *this;
    }

    template <typename T>
    template <typename T2>
    SharedPtr<T>& SharedPtr<T>::operator=(UniquePtr<T2>&& uniquePtr) noexcept
    {
        ptr = std::move(uniquePtr.Get());
        return *this;
    }

    template <typename T>
    SharedPtr<T> & SharedPtr<T>::operator=(T* pointer)
    {
        ptr = std::shared_ptr<T>(pointer);
        return *this;
    }

    template <typename T>
    SharedPtr<T> & SharedPtr<T>::operator=(SharedPtr& other)
    {
        ptr = other.ptr;
        return *this;
    }

    template <typename T>
    SharedPtr<T> & SharedPtr<T>::operator=(SharedPtr&& other) noexcept
    {
        ptr = std::move(other.ptr);
        return *this;
    }

    template <typename T>
    T* SharedPtr<T>::operator->() const noexcept
    {
        return ptr.operator->();
    }

    template <typename T>
    T& SharedPtr<T>::operator*() const noexcept
    {
        return ptr.operator*();
    }

    template <typename T>
    bool SharedPtr<T>::operator==(nullptr_t) const noexcept
    {
        return ptr == nullptr;
    }

    template <typename T>
    bool SharedPtr<T>::operator!=(nullptr_t) const noexcept
    {
        return ptr != nullptr;
    }

    template <typename T>
    SharedPtr<T>::operator bool() const noexcept
    {
        return Valid();
    }

    template <typename T>
    bool SharedPtr<T>::Valid() const
    {
        return ptr != nullptr;
    }

    template <typename T>
    T* SharedPtr<T>::Get() const
    {
        return ptr.get();
    }

    template <typename T>
    void SharedPtr<T>::Reset(T* pointer)
    {
        ptr.reset(pointer);
    }

    template <typename T>
    auto SharedPtr<T>::RefCount() const
    {
        return ptr.use_count();
    }

    template <typename T>
    std::shared_ptr<T> & SharedPtr<T>::GetStd()
    {
        return ptr;
    }

    template <typename T>
    template <typename T2>
    SharedPtr<T2> SharedPtr<T>::StaticCast()
    {
        return static_pointer_cast<T2, T>(ptr);
    }

    template <typename T>
    template <typename T2>
    SharedPtr<T2> SharedPtr<T>::DynamicCast()
    {
        return dynamic_pointer_cast<T2, T>(ptr);
    }

    template <typename T>
    template <typename T2>
    SharedPtr<T2> SharedPtr<T>::ReinterpretCast()
    {
        return reinterpret_pointer_cast<T2, T>(ptr);
    }

    template <typename T>
    template <typename T2>
    WeakPtr<T>::WeakPtr(SharedPtr<T2> & sharedPtr)
        : ptr(sharedPtr.GetStd())
    {
    }

    template <typename T>
    WeakPtr<T>::WeakPtr(WeakPtr& other)
        : ptr(other.ptr)
    {
    }

    template <typename T>
    WeakPtr<T>::WeakPtr(WeakPtr && other) noexcept
        : ptr(std::move(other.ptr))
    {
    }

    template <typename T>
    WeakPtr<T>::~WeakPtr() = default;

    template <typename T>
    template <typename T2>
    WeakPtr<T>& WeakPtr<T>::operator=(SharedPtr<T2> & sharedPtr)
    {
        ptr = sharedPtr.GetStd();
        return *this;
    }

    template <typename T>
    WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr & other)
    {
        ptr = other.ptr;
        return *this;
    }

    template <typename T>
    WeakPtr<T> & WeakPtr<T>::operator=(WeakPtr && other) noexcept
    {
        ptr = std::move(other.ptr);
        return *this;
    }

    template <typename T>
    void WeakPtr<T>::Reset()
    {
        ptr.reset();
    }

    template <typename T>
    bool WeakPtr<T>::Expired() const
    {
        return ptr.expired();
    }

    template <typename T>
    SharedPtr<T> WeakPtr<T>::Lock() const
    {
        return ptr.lock();
    }

    template <typename T>
    std::weak_ptr<T>& WeakPtr<T>::GetStd()
    {
        return ptr;
    }

    template <typename T, typename... Args>
    UniquePtr<T> MakeUnique(Args && ... args)
    {
        return Common::UniquePtr<T>(new T(std::forward<Args>(args)...));
    }

    template <typename T, typename... Args>
    SharedPtr<T> MakeShared(Args && ... args)
    {
        return Common::SharedPtr<T>(new T(std::forward<Args>(args)...));
    }
}
