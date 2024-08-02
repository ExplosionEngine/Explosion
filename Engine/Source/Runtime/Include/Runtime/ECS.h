//
// Created by johnk on 2024/8/2.
//

#pragma once

#include <entt/entt.hpp>

#include <Mirror/Meta.h>
#include <Mirror/Mirror.h>
#include <Runtime/Api.h>

namespace Runtime {
    using Entity = entt::entity;
    constexpr auto entityNull = entt::null;

    struct RUNTIME_API EClass() Component {
        EClassBody(Component)
    };

    struct RUNTIME_API EClass() State {
        EClassBody(State)
    };

    struct RUNTIME_API EClass() Event {
        EClassBody(Event)
    };

    struct RUNTIME_API EClass() System {
        EClassBody(System)
    };

    template <typename S>
    concept StateDerived = std::is_base_of_v<State, S>;

    template <typename C>
    concept CompDerived = std::is_base_of_v<Component, C>;

    template <typename E>
    concept EventDerived = std::is_base_of_v<Event, E>;

    template <typename S>
    concept SystemDerived = std::is_base_of_v<System, S>;

    template <typename T>
    struct SystemFuncTraits {};

    template <typename... T>
    struct Exclude {};
}

namespace Runtime {
    template <typename System, typename Ret>
    struct SystemFuncTraits<Ret(System::*)()> {
        using ListenEvent = void;
        using SignalEvent = Ret;
    };

    template <typename System, typename Ret>
    struct SystemFuncTraits<Ret(System::*)() const> {
        using ListenEvent = void;
        using SignalEvent = Ret;
    };

    template <typename System, typename Ret, typename Arg>
    struct SystemFuncTraits<Ret(System::*)(const Arg&)> {
        using ListenEvent = Arg;
        using SignalEvent = Ret;
    };

    template <typename System, typename Ret, typename Arg>
    struct SystemFuncTraits<Ret(System::*)(const Arg&) const> {
        using ListenEvent = Arg;
        using SignalEvent = Ret;
    };
}
