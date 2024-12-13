//
// Created by johnk on 2024/11/5.
//

#pragma once

#include <utility>
#include <vector>
#include <functional>

#include <Common/Debug.h>

#define IMPL_INDEX_TO_STD_PLACEHOLDER(I) \
    template <> struct IndexToStdPlaceholder<I> { static constexpr auto value = std::placeholders::_##I; }; \

namespace Common::Internal {
    template <size_t I> struct IndexToStdPlaceholder {};

    IMPL_INDEX_TO_STD_PLACEHOLDER(1)
    IMPL_INDEX_TO_STD_PLACEHOLDER(2)
    IMPL_INDEX_TO_STD_PLACEHOLDER(3)
    IMPL_INDEX_TO_STD_PLACEHOLDER(4)
    IMPL_INDEX_TO_STD_PLACEHOLDER(5)
    IMPL_INDEX_TO_STD_PLACEHOLDER(6)
    IMPL_INDEX_TO_STD_PLACEHOLDER(7)
    IMPL_INDEX_TO_STD_PLACEHOLDER(8)
    IMPL_INDEX_TO_STD_PLACEHOLDER(9)
    IMPL_INDEX_TO_STD_PLACEHOLDER(10)
}

namespace Common {
    using CallbackHandle = size_t;
    
    template <typename... T>
    class Delegate {
    public:
        Delegate();

        template <auto F> CallbackHandle BindStatic();
        template <auto F, typename C> CallbackHandle BindMember(C& inObj);
        template <typename F> CallbackHandle BindLambda(F&& inLambda);
        template <typename... Args> void Broadcast(Args&&... inArgs) const;
        void Unbind(CallbackHandle inHandle);
        size_t Count() const;
        void Reset();

    private:
        template <auto F, size_t... I> void BindStaticInternal(CallbackHandle inHandle, std::index_sequence<I...>);
        template <auto F, typename C, size_t... I> void BindMemberInternal(CallbackHandle inHandle, C& inObj, std::index_sequence<I...>);
        template <typename F, size_t... I> void BindLambdaInternal(CallbackHandle inHandle, F&& inLambda, std::index_sequence<I...>);

        CallbackHandle counter;
        std::vector<std::pair<CallbackHandle, std::function<void(T...)>>> receivers;
    };
}

namespace Common {
    template <typename... T>
    Delegate<T...>::Delegate()
        : counter(0)
    {
    }

    template <typename... T>
    template <auto F>
    CallbackHandle Delegate<T...>::BindStatic()
    {
        const auto handle = counter++;
        BindStaticInternal<F>(handle, std::make_index_sequence<sizeof...(T)>());
        return handle;
    }

    template <typename... T>
    template <auto F, typename C>
    CallbackHandle Delegate<T...>::BindMember(C& inObj)
    {
        const auto handle = counter++;
        BindMemberInternal<F, C>(handle, inObj, std::make_index_sequence<sizeof...(T)>());
        return handle;
    }

    template <typename... T>
    template <typename F>
    CallbackHandle Delegate<T...>::BindLambda(F&& inLambda)
    {
        const auto handle = counter++;
        BindLambdaInternal(handle, std::forward<F>(inLambda), std::make_index_sequence<sizeof...(T)>());
        return handle;
    }

    template <typename... T>
    template <typename... Args>
    void Delegate<T...>::Broadcast(Args&&... inArgs) const
    {
        for (const auto& [_, receiver] : receivers) {
            receiver(std::forward<Args>(inArgs)...);
        }
    }

    template <typename... T>
    void Delegate<T...>::Unbind(CallbackHandle inHandle)
    {
        auto iter = std::find_if(receivers.begin(), receivers.end(), [&](const auto& pair) -> bool { return pair.first == inHandle; });
        Assert(iter != receivers.end());
        receivers.erase(iter);
    }

    template <typename... T>
    size_t Delegate<T...>::Count() const
    {
        return receivers.size();
    }

    template <typename... T>
    void Delegate<T...>::Reset()
    {
        counter = 0;
        receivers.clear();
    }

    template <typename... T>
    template <auto F, size_t... I>
    void Delegate<T...>::BindStaticInternal(CallbackHandle inHandle, std::index_sequence<I...>)
    {
        receivers.emplace_back(inHandle, std::bind(F, Internal::IndexToStdPlaceholder<I + 1>::value...));
    }

    template <typename... T>
    template <auto F, typename C, size_t... I>
    void Delegate<T...>::BindMemberInternal(CallbackHandle inHandle, C& inObj, std::index_sequence<I...>)
    {
        receivers.emplace_back(inHandle, std::bind(F, &inObj, Internal::IndexToStdPlaceholder<I + 1>::value...));
    }

    template <typename... T>
    template <typename F, size_t... I>
    void Delegate<T...>::BindLambdaInternal(CallbackHandle inHandle, F&& inLambda, std::index_sequence<I...>)
    {
        receivers.emplace_back(inHandle, std::bind(inLambda, Internal::IndexToStdPlaceholder<I + 1>::value...));
    }
} // namespace Common
