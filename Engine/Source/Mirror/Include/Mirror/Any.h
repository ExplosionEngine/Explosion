//
// Created by johnk on 2022/9/5.
//

#pragma once

#include <vector>
#include <functional>

#include <Common/Debug.h>
#include <Mirror/TypeInfo.h>

namespace Mirror::Internal {
    struct AnyRtti {
        using DetorFunc = void(void*) noexcept;
        using CopyFunc = void(void*, const void*);
        using MoveFunc = void(void*, void*) noexcept;

        template <class T>
        static void DetorImpl(void* const object) noexcept {
            reinterpret_cast<T*>(object)->~T();
        }

        template <class T>
        static void CopyImpl(void* const object, const void* const other) {
            new(object) T(*reinterpret_cast<const T*>(other));
        }

        template <class T>
        static void MoveImpl(void* const object, void* const other) noexcept {
            new(object) T(std::move(*reinterpret_cast<const T*>(other)));
        }

        DetorFunc* detor;
        CopyFunc* copy;
        MoveFunc* move;
    };

    template <class T>
    inline constexpr AnyRtti anyRttiImpl = {
        &AnyRtti::DetorImpl<T>,
        &AnyRtti::CopyImpl<T>,
        &AnyRtti::MoveImpl<T>
    };
}

namespace Mirror {
    class Any {
    public:
        Any() = default;

        ~Any()
        {
            if (rtti != nullptr) {
                rtti->detor(data.data());
            }
        }

        template <typename T>
        Any(T&& value) // NOLINT
        {
            ConstructValue(std::forward<T>(value));
        }

        template <typename T>
        Any(const std::reference_wrapper<T>& ref) // NOLINT
        {
            ConstructRef(ref);
        }

        template <typename T>
        Any(std::reference_wrapper<T>&& ref) // NOLINT
        {
            ConstructRef(std::move(ref));
        }

        Any(const Any& inAny)
        {
            typeInfo = inAny.typeInfo;
            rtti = inAny.rtti;
            data.resize(inAny.data.size());
            rtti->copy(data.data(), inAny.data.data());
        }

        Any(Any&& inAny) noexcept
        {
            typeInfo = inAny.typeInfo;
            rtti = inAny.rtti;
            data.resize(inAny.data.size());
            rtti->move(data.data(), inAny.data.data());
        }

        template <typename T>
        Any& operator=(T&& value)
        {
            Reset();
            ConstructValue(std::forward<T>(value));
            return *this;
        }

        template <typename T>
        Any& operator=(const std::reference_wrapper<T>& ref)
        {
            Reset();
            ConstructRef(ref);
            return *this;
        }

        template <typename T>
        Any& operator=(std::reference_wrapper<T>&& ref)
        {
            Reset();
            ConstructRef(ref);
            return *this;
        }

        Any& operator=(const Any& inAny)
        {
            if (&inAny == this) {
                return *this;
            }
            Reset();
            typeInfo = inAny.typeInfo;
            rtti = inAny.rtti;
            rtti->copy(data.data(), inAny.data.data());
            return *this;
        }

        Any& operator=(Any&& inAny) noexcept
        {
            Reset();
            typeInfo = inAny.typeInfo;
            rtti = inAny.rtti;
            rtti->move(data.data(), inAny.data.data());
            return *this;
        }

        [[nodiscard]] size_t Size() const
        {
            return data.size();
        }

        [[nodiscard]] const void* Data() const
        {
            return data.data();
        }

        [[nodiscard]] bool IsReference() const
        {
            return typeInfo->isLValueReference;
        }

        // T -> T: true
        // T -> const T: true
        // T -> T&: true
        // T -> const T&: true
        //
        // const T -> T: false
        // const T -> const T: true
        // const T -> T&: false
        // const T -> const T&: true
        //
        // T& -> T: true
        // T& -> const T: true
        // T& -> T&: true
        // T& -> const T&: true
        //
        // const T& -> T: false
        // const T& -> const T: true
        // const T& -> T&: false
        // const T& -> const T&: true
        template <typename T>
        [[nodiscard]] bool Castable()
        {
            return CastableInternal<T>(typeInfo);
        }

        // T const -> T: false
        // T const -> const T: true
        // T const -> T&: false
        // T const -> const T&: true
        //
        // const T -> T: false
        // const T -> const T: true
        // const T -> T&: false
        // const T -> const T&: true
        //
        // T& const -> T: true
        // T& const -> const T: true
        // T& const -> T&: true
        // T& const -> const T&: true
        //
        // const T& const -> T: false
        // const T& const -> const T: true
        // const T& const -> T&: false
        // const T& const -> const T&: true
        template <typename T>
        [[nodiscard]] bool Castable() const
        {
            return CastableInternal<T>(typeInfo->addConst());
        }

        template <typename T>
        T As()
        {
            Assert(Castable<T>());
            if (IsReference()) {
                return reinterpret_cast<std::reference_wrapper<std::remove_reference_t<T>>*>(data.data())->get();
            } else {
                return *reinterpret_cast<std::remove_reference_t<T>*>(data.data());
            }
        }

        template <typename T>
        T As() const
        {
            Assert(Castable<T>());
            if (IsReference()) {
                return reinterpret_cast<std::add_const_t<std::reference_wrapper<std::remove_reference_t<T>>>*>(data.data())->get();
            } else {
                void* dataPtr = const_cast<uint8_t*>(data.data());
                return *reinterpret_cast<std::remove_reference_t<T>*>(dataPtr);
            }
        }

        template <typename T>
        T* TryAs()
        {
            Assert(!IsReference());
            if (Castable<T>()) {
                return reinterpret_cast<std::remove_reference_t<T>*>(data.data());
            } else {
                return nullptr;
            }
        }

        template <typename T>
        T* TryAs() const
        {
            Assert(!IsReference());
            if (Castable<T>()) {
                void* dataPtr = const_cast<uint8_t*>(data.data());
                return *reinterpret_cast<std::remove_reference_t<T>*>(dataPtr);
            } else {
                return nullptr;
            }
        }

        void Reset()
        {
            if (rtti != nullptr) {
                rtti->detor(data.data());
            }
            typeInfo = nullptr;
            rtti = nullptr;
        }

    private:
        template <typename T>
        [[nodiscard]] static bool CastableInternal(TypeInfo* actualTypeInfo)
        {
            if (actualTypeInfo->isLValueReference) {
                auto* removeRefType = actualTypeInfo->removeRef();
                if (removeRefType->isConst) {
                    return removeRefType->id == GetTypeInfo<std::remove_reference_t<T>>()->id;
                } else {
                    return removeRefType->id == GetTypeInfo<std::remove_cvref_t<T>>()->id;
                }
            } else {
                if (actualTypeInfo->isConst) {
                    return actualTypeInfo->id == GetTypeInfo<std::remove_reference_t<T>>()->id;
                } else {
                    return actualTypeInfo->id == GetTypeInfo<std::remove_cvref_t<T>>()->id;
                }
            }
        }

        template <typename T>
        void ConstructValue(T&& value)
        {
            using RemoveCVRefType = std::remove_cvref_t<T>;
            using RemoveRefType = std::remove_reference_t<T>;

            typeInfo = GetTypeInfo<RemoveRefType>();
            rtti = &Internal::anyRttiImpl<RemoveCVRefType>;

            data.resize(sizeof(RemoveCVRefType));
            new(data.data()) RemoveCVRefType(std::forward<T>(value));
        }

        template <typename T>
        void ConstructRef(const std::reference_wrapper<T>& ref)
        {
            using RefWrapperType = std::reference_wrapper<T>;
            using RefType = T&;

            typeInfo = GetTypeInfo<RefType>();
            rtti = &Internal::anyRttiImpl<RefWrapperType>;

            data.resize(sizeof(RefWrapperType));
            new(data.data()) RefWrapperType(ref);
        }

        TypeInfo* typeInfo = nullptr;
        const Internal::AnyRtti* rtti;
        std::vector<uint8_t> data;
    };
}
