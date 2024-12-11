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
    using ReceiverHandle = size_t;
    
    template <typename... T>
    class Event {
    public:
        Event();

        template <auto F> ReceiverHandle BindStatic();
        template <auto F, typename C> ReceiverHandle BindMember(C& inObj);
        template <typename F> ReceiverHandle BindLambda(F&& inLambda);
        template <typename... Args> void Broadcast(Args&&... inArgs) const;
        void Unbind(ReceiverHandle inHandle);
        size_t ReceiversCount() const;
        void Reset();

    private:
        template <auto F, size_t... I> void BindStaticInternal(ReceiverHandle inHandle, std::index_sequence<I...>);
        template <auto F, typename C, size_t... I> void BindMemberInternal(ReceiverHandle inHandle, C& inObj, std::index_sequence<I...>);
        template <typename F, size_t... I> void BindLambdaInternal(ReceiverHandle inHandle, F&& inLambda, std::index_sequence<I...>);

        ReceiverHandle counter;
        std::vector<std::pair<ReceiverHandle, std::function<void(T...)>>> receivers;
    };
}

namespace Common {
    template <typename... T>
    Event<T...>::Event()
        : counter(0)
    {
    }

    template <typename... T>
    template <auto F>
    ReceiverHandle Event<T...>::BindStatic()
    {
        const auto handle = counter++;
        BindStaticInternal<F>(handle, std::make_index_sequence<sizeof...(T)>());
        return handle;
    }

    template <typename... T>
    template <auto F, typename C>
    ReceiverHandle Event<T...>::BindMember(C& inObj)
    {
        const auto handle = counter++;
        BindMemberInternal<F, C>(handle, inObj, std::make_index_sequence<sizeof...(T)>());
        return handle;
    }

    template <typename... T>
    template <typename F>
    ReceiverHandle Event<T...>::BindLambda(F&& inLambda)
    {
        const auto handle = counter++;
        BindLambdaInternal(handle, std::forward<F>(inLambda), std::make_index_sequence<sizeof...(T)>());
        return handle;
    }

    template <typename... T>
    template <typename... Args>
    void Event<T...>::Broadcast(Args&&... inArgs) const
    {
        for (const auto& [_, receiver] : receivers) {
            receiver(std::forward<Args>(inArgs)...);
        }
    }

    template <typename... T>
    void Event<T...>::Unbind(ReceiverHandle inHandle)
    {
        auto iter = std::find_if(receivers.begin(), receivers.end(), [&](const auto& pair) -> bool { return pair.first == inHandle; });
        Assert(iter != receivers.end());
        receivers.erase(iter);
    }

    template <typename... T>
    size_t Event<T...>::ReceiversCount() const
    {
        return receivers.size();
    }

    template <typename... T>
    void Event<T...>::Reset()
    {
        counter = 0;
        receivers.clear();
    }

    template <typename... T>
    template <auto F, size_t... I>
    void Event<T...>::BindStaticInternal(ReceiverHandle inHandle, std::index_sequence<I...>)
    {
        receivers.emplace_back(inHandle, std::bind(F, Internal::IndexToStdPlaceholder<I + 1>::value...));
    }

    template <typename... T>
    template <auto F, typename C, size_t... I>
    void Event<T...>::BindMemberInternal(ReceiverHandle inHandle, C& inObj, std::index_sequence<I...>)
    {
        receivers.emplace_back(inHandle, std::bind(F, &inObj, Internal::IndexToStdPlaceholder<I + 1>::value...));
    }

    template <typename... T>
    template <typename F, size_t... I>
    void Event<T...>::BindLambdaInternal(ReceiverHandle inHandle, F&& inLambda, std::index_sequence<I...>)
    {
        receivers.emplace_back(inHandle, std::bind(inLambda, Internal::IndexToStdPlaceholder<I + 1>::value...));
    }
} // namespace Common
