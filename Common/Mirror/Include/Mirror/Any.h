//
// Created by johnk on 2021/9/4.
//

#ifndef EXPLOSION_ANY_H
#define EXPLOSION_ANY_H

#include <memory>
#include <variant>
#include <functional>

namespace {
    enum class StorageCategory {
        SMALL,
        BIG
    };

    inline constexpr size_t MAX_SMALL_STORAGE_SIZE = sizeof(void*) * 4;

    template <typename T>
    inline constexpr bool IsSmallStorage()
    {
        return sizeof(T) <= MAX_SMALL_STORAGE_SIZE && std::is_trivially_copyable_v<T>;
    }

    struct BigStorageRtti {
        std::function<void*(const void*)> copyFunc;
        std::function<void*(const void*)> moveFunc;
        std::function<void(void*)> destroyFunc;
    };

    template <typename T>
    struct BigStorageRttiImpl {
        static void* Copy(const void* src)
        {
            return new T(*static_cast<T*>(src));
        }

        static void* Move(const void* src)
        {
            return new T(std::move(*static_cast<T*>(src)));
        }

        static void Destroy(void* src)
        {
            delete (static_cast<T*>(src));
        }
    };

    template <typename T>
    static const BigStorageRtti bigStorageRtti {
        &BigStorageRttiImpl<T>::Copy,
        &BigStorageRttiImpl<T>::Move,
        &BigStorageRttiImpl<T>::Destroy
    };

    struct SmallStorage {
        uint8_t memory[MAX_SMALL_STORAGE_SIZE];
    };

    struct BigStorage {
        void* memory;
    };

    using Storage = std::variant<SmallStorage, BigStorage>;
}

namespace Explosion::Mirror {
    class Any {
    public:
        // TODO

    private:
        // TODO
    };
}

#endif //EXPLOSION_ANY_H
