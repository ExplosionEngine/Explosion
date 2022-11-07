//
// Created by johnk on 2022/9/5.
//

#pragma once

#include <vector>
#include <functional>

#include <Common/Debug.h>
#include <Mirror/TypeInfo.h>

namespace Mirror {
    class Any {
    public:
        Any() = default;

        ~Any()
        {
            if (detor) {
                detor();
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
            isReference = inAny.isReference;
            typeInfo = inAny.typeInfo;
            detor = inAny.detor;
            copy = inAny.copy;
            move = inAny.move;
            data = inAny.data;
        }

        Any(Any&& inAny) noexcept
        {
            isReference = inAny.isReference;
            typeInfo = inAny.typeInfo;
            detor = inAny.detor;
            copy = inAny.copy;
            move = inAny.move;
            data = std::move(inAny.data);
        }

        template <typename T>
        Any& operator=(T&& value)
        {
            ConstructValue(std::forward<T>(value));
            return *this;
        }

        template <typename T>
        Any& operator=(const std::reference_wrapper<T>& ref)
        {
            ConstructRef(ref);
            return *this;
        }

        template <typename T>
        Any& operator=(std::reference_wrapper<T>&& ref)
        {
            ConstructRef(ref);
            return *this;
        }

        Any& operator=(const Any& inAny)
        {
            if (&inAny == this) {
                return *this;
            }
            Assert(typeInfo->id == inAny.typeInfo->id);
            copy(inAny.data.data());
            return *this;
        }

        Any& operator=(Any&& inAny) noexcept
        {
            Assert(typeInfo->id == inAny.typeInfo->id);
            move(inAny.data.data());
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
            return isReference;
        }

        template <typename T>
        [[nodiscard]] bool CanCastTo() const
        {
            if (isReference) {
                bool result = typeInfo->id == GetTypeInfo<std::remove_reference_t<T>>()->id;
                if (!typeInfo->isConst) {
                    result = result || GetTypeInfo<std::remove_cvref_t<T>>()->id;
                }
                return result;
            }
            return typeInfo->id == GetTypeInfo<std::remove_cvref_t<T>>()->id;
        }

        template <typename T>
        T CastTo()
        {
            Assert(CanCastTo<T>());
            return isReference ? reinterpret_cast<std::reference_wrapper<std::remove_reference_t<T>>*>(data.data())->get() : *reinterpret_cast<std::remove_cvref_t<T>*>(data.data());
        }

        template <typename T>
        T* TryCastTo()
        {
            Assert(!isReference);
            return CanCastTo<T>() ? reinterpret_cast<std::remove_cvref_t<T>*>(data.data()) : nullptr;
        }

    private:
        template <typename T>
        void ConstructValue(T&& value)
        {
            using RemoveCVRefType = std::remove_cvref_t<T>;

            if (detor) {
                detor();
            }
            isReference = false;
            typeInfo = GetTypeInfo<RemoveCVRefType>();
            detor = [this]() -> void { reinterpret_cast<RemoveCVRefType*>(data.data())->~RemoveCVRefType(); };
            copy = [this](const void* inData) -> void { new(data.data()) RemoveCVRefType(*reinterpret_cast<const RemoveCVRefType*>(inData)); };
            move = [this](const void* inData) -> void { new(data.data()) RemoveCVRefType(std::move(*reinterpret_cast<const RemoveCVRefType*>(inData))); };
            data.resize(sizeof(RemoveCVRefType));
            if constexpr (std::is_rvalue_reference_v<T>) {
                move(&value);
            } else {
                copy(&value);
            }
        }

        template <typename T>
        void ConstructRef(const std::reference_wrapper<T>& ref)
        {
            using RefWrapperType = std::reference_wrapper<T>;

            if (detor) {
                detor();
            }
            isReference = true;
            typeInfo = GetTypeInfo<T>();
            detor = []() -> void {};
            copy = [this](const void* inData) -> void { new(data.data()) RefWrapperType(*reinterpret_cast<const RefWrapperType*>(inData)); };
            move = [this](const void* inData) -> void { new(data.data()) RefWrapperType(std::move(*reinterpret_cast<const RefWrapperType*>(inData))); };
            data.resize(sizeof(RefWrapperType));
            copy(&ref);
        }

        bool isReference = false;
        TypeInfo* typeInfo = nullptr;
        std::function<void()> detor;
        std::function<void(const void*)> move;
        std::function<void(const void*)> copy;
        std::vector<uint8_t> data;
    };
}
