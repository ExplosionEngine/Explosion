//
// Created by johnk on 2021/9/4.
//

#ifndef EXPLOSION_ANY_H
#define EXPLOSION_ANY_H

#include <memory>
#include <variant>
#include <functional>

#include <Mirror/Exception.h>
#include <Mirror/Type.h>

namespace {
    enum class StorageCategory {
        SMALL,
        BIG
    };

    inline constexpr size_t MAX_SMALL_STORAGE_SIZE = sizeof(void*) * 4;

    template <typename T>
    inline constexpr StorageCategory GetStorageCategory()
    {
        return sizeof(T) <= MAX_SMALL_STORAGE_SIZE && std::is_trivially_copyable_v<T> ?
            StorageCategory::SMALL :
            StorageCategory::BIG;
    }

    struct BigStorageRtti {
        std::function<void*(void*)> copyFunc;
        std::function<void*(void*)> moveFunc;
        std::function<void(void*)> destroyFunc;
    };

    template <typename T>
    struct BigStorageRttiImpl {
        static void* Copy(void* src)
        {
            return new T(*static_cast<T*>(src));
        }

        static void* Move(void* src)
        {
            return new T(std::move(*static_cast<T*>(src)));
        }

        static void Destroy(void* src)
        {
            delete (static_cast<T*>(src));
        }
    };

    template <typename T>
    static BigStorageRtti bigStorageRtti {
        &BigStorageRttiImpl<T>::Copy,
        &BigStorageRttiImpl<T>::Move,
        &BigStorageRttiImpl<T>::Destroy
    };

    struct SmallStorage {
        uint8_t memory[MAX_SMALL_STORAGE_SIZE];
    };

    struct BigStorage {
        void* memory;
        BigStorageRtti* rtti;
    };

    using Storage = std::variant<SmallStorage, BigStorage>;
}

namespace Explosion::Mirror {
    class Any {
    public:
        Any() : storageCategory(StorageCategory::SMALL), typeId(0) {}

        template <typename T>
        Any(T&& value)
        {
            Construct(std::forward<T>(value));
        }

        template <typename T>
        Any& operator=(T&& value)
        {
            Construct(std::forward<T>(value));
            return *this;
        }

        Any(Any& any) : storageCategory(any.storageCategory), typeId(any.typeId)
        {
            if (storageCategory == StorageCategory::SMALL) {
                storage = SmallStorage {};
                auto& sThis = std::get<SmallStorage>(storage);
                auto& sAny = std::get<SmallStorage>(any.storage);
                memcpy(sThis.memory, sAny.memory, MAX_SMALL_STORAGE_SIZE);
            } else {
                storage = BigStorage {};
                auto& sThis = std::get<BigStorage>(storage);
                auto& sAny = std::get<BigStorage>(any.storage);
                sThis.memory = sAny.rtti->copyFunc(sAny.memory);
                sThis.rtti = sAny.rtti;
            }
        }

        Any(Any&& any)  noexcept : storageCategory(any.storageCategory), typeId(any.typeId)
        {
            if (storageCategory == StorageCategory::SMALL) {
                storage = SmallStorage {};
                auto& sThis = std::get<SmallStorage>(storage);
                auto& sAny = std::get<SmallStorage>(any.storage);
                std::swap(sThis.memory, sAny.memory);
            } else {
                storage = BigStorage {};
                auto& sThis = std::get<BigStorage>(storage);
                auto& sAny = std::get<BigStorage>(any.storage);
                sThis.memory = sAny.rtti->moveFunc(sAny.memory);
                sThis.rtti = sAny.rtti;
            }
        }

        ~Any()
        {
            if (storageCategory == StorageCategory::SMALL) {
                auto& s = std::get<SmallStorage>(storage);
                memset(s.memory, 0, MAX_SMALL_STORAGE_SIZE);
            } else {
                auto& s = std::get<BigStorage>(storage);
                s.rtti->destroyFunc(s.memory);
            }
        }

        template <typename T>
        T CastTo()
        {
            return *CastToPointer<T>();
        }

        template <typename T>
        T* CastToPointer()
        {
            if (Internal::RuntimeTraits<T>::HashCode() != typeId) {
                throw BadAnyCastException {};
            }
            return static_cast<T*>(RawValue());
        }

        void* RawValue()
        {
            return storageCategory == StorageCategory::SMALL ?
                static_cast<void*>(std::get<SmallStorage>(storage).memory) :
                std::get<BigStorage>(storage).memory;
        }

    private:
        template <typename T>
        void Construct(T&& value)
        {
            storageCategory = GetStorageCategory<T>();
            typeId = Internal::RuntimeTraits<T>().HashCode();
            if (storageCategory == StorageCategory::SMALL) {
                storage = SmallStorage {};
                auto& s = std::get<SmallStorage>(storage);
                memcpy(s.memory, &value, sizeof(T));
            } else {
                storage = BigStorage {};
                auto& s = std::get<BigStorage>(storage);
                s.memory = new T(std::forward<T>(value));
                s.rtti = &bigStorageRtti<T>;
            }
        }

        StorageCategory storageCategory;
        Storage storage;
        size_t typeId;
    };
}

#endif //EXPLOSION_ANY_H
