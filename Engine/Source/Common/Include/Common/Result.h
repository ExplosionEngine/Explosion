//
// Created by johnk on 2026/6/20.
//

#pragma once

#include <variant>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

#include <Common/Debug.h>

namespace Common::Internal {
    template <typename T>
    struct OkWrapper {
        T value;
    };

    template <>
    struct OkWrapper<void> {};

    template <typename E>
    struct ErrWrapper {
        E error;
    };
}

namespace Common {
    template <typename T>
    Internal::OkWrapper<std::decay_t<T>> Ok(T&& inValue);

    Internal::OkWrapper<void> Ok();

    template <typename E>
    Internal::ErrWrapper<std::decay_t<E>> Err(E&& inError);

    // A Rust-like Result<T, E>: holds either a success value of type T or an error of type E, never both. Build one with
    // the Ok()/Err() helpers (Result<void, E> mirrors Rust's Result<(), E> and is constructed from the argument-less
    // Ok()). Value()/Error() hand out references to the active alternative; the Unwrap()/Expect() family moves or copies
    // it out; Map()/AndThen() and friends chain transformations. Every accessor asserts the active alternative, so check
    // IsOk()/IsErr() first when a branch is possible. The value-side accessors take a defaulted T2 = T template
    // parameter purely so the void specialization stays well-formed (a member's signature is only ill-formed once it is
    // actually instantiated, which never happens for the disabled overloads).
    template <typename T, typename E>
    class Result {
    public:
        using ValueType = T;
        using ErrorType = E;

        Result(Internal::OkWrapper<T> inOk); // NOLINT
        Result(Internal::ErrWrapper<E> inErr); // NOLINT

        bool IsOk() const;
        bool IsErr() const;
        explicit operator bool() const;

        template <typename T2 = T> requires (!std::is_void_v<T2>) T2& Value() &;
        template <typename T2 = T> requires (!std::is_void_v<T2>) const T2& Value() const&;
        E& Error() &;
        const E& Error() const&;

        template <typename T2 = T> requires (!std::is_void_v<T2>) T2 Unwrap() const&;
        template <typename T2 = T> requires (!std::is_void_v<T2>) T2 Unwrap() &&;
        E UnwrapErr() const&;
        E UnwrapErr() &&;
        template <typename T2 = T> requires (!std::is_void_v<T2>) T2 Expect(std::string_view inReason) const&;
        template <typename T2 = T> requires (!std::is_void_v<T2>) T2 Expect(std::string_view inReason) &&;
        E ExpectErr(std::string_view inReason) const&;
        E ExpectErr(std::string_view inReason) &&;

        template <typename T2 = T> requires (!std::is_void_v<T2>) T2 UnwrapOr(T2 inFallback) const&;
        template <typename F, typename T2 = T> requires (!std::is_void_v<T2>) T2 UnwrapOrElse(F&& inFunc) const&;

        template <typename F, typename T2 = T> requires (!std::is_void_v<T2>) Result<std::invoke_result_t<F, const T2&>, E> Map(F&& inFunc) const&;
        template <typename F> Result<T, std::invoke_result_t<F, const E&>> MapErr(F&& inFunc) const&;
        template <typename F, typename T2 = T> requires (!std::is_void_v<T2>) std::invoke_result_t<F, const T2&> AndThen(F&& inFunc) const&;
        template <typename F> std::invoke_result_t<F, const E&> OrElse(F&& inFunc) const&;

        template <typename T2 = T> requires (!std::is_void_v<T2>) std::optional<T2> ToOptional() const&;

    private:
        using StoredValueType = std::conditional_t<std::is_void_v<T>, Internal::OkWrapper<void>, T>;

        static StoredValueType ExtractOk(Internal::OkWrapper<T>&& inOk);

        std::variant<StoredValueType, E> storage;
    };
}

namespace Common {
    template <typename T>
    Internal::OkWrapper<std::decay_t<T>> Ok(T&& inValue)
    {
        return { std::forward<T>(inValue) };
    }

    inline Internal::OkWrapper<void> Ok()
    {
        return {};
    }

    template <typename E>
    Internal::ErrWrapper<std::decay_t<E>> Err(E&& inError)
    {
        return { std::forward<E>(inError) };
    }

    template <typename T, typename E>
    Result<T, E>::Result(Internal::OkWrapper<T> inOk)
        : storage(std::in_place_index<0>, ExtractOk(std::move(inOk)))
    {
    }

    template <typename T, typename E>
    Result<T, E>::Result(Internal::ErrWrapper<E> inErr)
        : storage(std::in_place_index<1>, std::move(inErr.error))
    {
    }

    template <typename T, typename E>
    bool Result<T, E>::IsOk() const
    {
        return storage.index() == 0;
    }

    template <typename T, typename E>
    bool Result<T, E>::IsErr() const
    {
        return storage.index() == 1;
    }

    template <typename T, typename E>
    Result<T, E>::operator bool() const
    {
        return IsOk();
    }

    template <typename T, typename E>
    template <typename T2> requires (!std::is_void_v<T2>)
    T2& Result<T, E>::Value() &
    {
        Assert(IsOk());
        return std::get<0>(storage);
    }

    template <typename T, typename E>
    template <typename T2> requires (!std::is_void_v<T2>)
    const T2& Result<T, E>::Value() const&
    {
        Assert(IsOk());
        return std::get<0>(storage);
    }

    template <typename T, typename E>
    E& Result<T, E>::Error() &
    {
        Assert(IsErr());
        return std::get<1>(storage);
    }

    template <typename T, typename E>
    const E& Result<T, E>::Error() const&
    {
        Assert(IsErr());
        return std::get<1>(storage);
    }

    template <typename T, typename E>
    template <typename T2> requires (!std::is_void_v<T2>)
    T2 Result<T, E>::Unwrap() const&
    {
        Assert(IsOk());
        return std::get<0>(storage);
    }

    template <typename T, typename E>
    template <typename T2> requires (!std::is_void_v<T2>)
    T2 Result<T, E>::Unwrap() &&
    {
        Assert(IsOk());
        return std::get<0>(std::move(storage));
    }

    template <typename T, typename E>
    E Result<T, E>::UnwrapErr() const&
    {
        Assert(IsErr());
        return std::get<1>(storage);
    }

    template <typename T, typename E>
    E Result<T, E>::UnwrapErr() &&
    {
        Assert(IsErr());
        return std::get<1>(std::move(storage));
    }

    template <typename T, typename E>
    template <typename T2> requires (!std::is_void_v<T2>)
    T2 Result<T, E>::Expect(std::string_view inReason) const&
    {
        AssertWithReason(IsOk(), inReason);
        return std::get<0>(storage);
    }

    template <typename T, typename E>
    template <typename T2> requires (!std::is_void_v<T2>)
    T2 Result<T, E>::Expect(std::string_view inReason) &&
    {
        AssertWithReason(IsOk(), inReason);
        return std::get<0>(std::move(storage));
    }

    template <typename T, typename E>
    E Result<T, E>::ExpectErr(std::string_view inReason) const&
    {
        AssertWithReason(IsErr(), inReason);
        return std::get<1>(storage);
    }

    template <typename T, typename E>
    E Result<T, E>::ExpectErr(std::string_view inReason) &&
    {
        AssertWithReason(IsErr(), inReason);
        return std::get<1>(std::move(storage));
    }

    template <typename T, typename E>
    template <typename T2> requires (!std::is_void_v<T2>)
    T2 Result<T, E>::UnwrapOr(T2 inFallback) const&
    {
        return IsOk() ? std::get<0>(storage) : std::move(inFallback);
    }

    template <typename T, typename E>
    template <typename F, typename T2> requires (!std::is_void_v<T2>)
    T2 Result<T, E>::UnwrapOrElse(F&& inFunc) const&
    {
        return IsOk() ? std::get<0>(storage) : std::forward<F>(inFunc)(std::get<1>(storage));
    }

    template <typename T, typename E>
    template <typename F, typename T2> requires (!std::is_void_v<T2>)
    Result<std::invoke_result_t<F, const T2&>, E> Result<T, E>::Map(F&& inFunc) const&
    {
        if (IsErr()) {
            return Err(std::get<1>(storage));
        }
        return Ok(std::forward<F>(inFunc)(std::get<0>(storage)));
    }

    template <typename T, typename E>
    template <typename F>
    Result<T, std::invoke_result_t<F, const E&>> Result<T, E>::MapErr(F&& inFunc) const&
    {
        if (IsErr()) {
            return Err(std::forward<F>(inFunc)(std::get<1>(storage)));
        }
        if constexpr (std::is_void_v<T>) {
            return Ok();
        } else {
            return Ok(std::get<0>(storage));
        }
    }

    template <typename T, typename E>
    template <typename F, typename T2> requires (!std::is_void_v<T2>)
    std::invoke_result_t<F, const T2&> Result<T, E>::AndThen(F&& inFunc) const&
    {
        if (IsErr()) {
            return Err(std::get<1>(storage));
        }
        return std::forward<F>(inFunc)(std::get<0>(storage));
    }

    template <typename T, typename E>
    template <typename F>
    std::invoke_result_t<F, const E&> Result<T, E>::OrElse(F&& inFunc) const&
    {
        if (IsErr()) {
            return std::forward<F>(inFunc)(std::get<1>(storage));
        }
        if constexpr (std::is_void_v<T>) {
            return Ok();
        } else {
            return Ok(std::get<0>(storage));
        }
    }

    template <typename T, typename E>
    template <typename T2> requires (!std::is_void_v<T2>)
    std::optional<T2> Result<T, E>::ToOptional() const&
    {
        if (IsErr()) {
            return std::nullopt;
        }
        return std::get<0>(storage);
    }

    template <typename T, typename E>
    typename Result<T, E>::StoredValueType Result<T, E>::ExtractOk(Internal::OkWrapper<T>&& inOk)
    {
        if constexpr (std::is_void_v<T>) {
            return Internal::OkWrapper<void> {};
        } else {
            return std::move(inOk.value);
        }
    }
}
