//
// Created by Zach Lee on 2021/9/12.
//

#ifndef EXPLOSION_FLAGS_H
#define EXPLOSION_FLAGS_H

namespace Explosion {

    /**
     * common defines
     */
    using Flags = uint32_t;

    template<typename E>
    std::enable_if_t<std::is_enum_v<E>, Flags> FlagsCast(const E& e)
    {
        return static_cast<Flags>(e);
    }

    template<typename E>
    std::enable_if_t<std::is_enum_v<E>, Flags> operator|(const E& l, const E& r)
    {
        return FlagsCast(l) | FlagsCast(r);
    }

    template<typename E>
    std::enable_if_t<std::is_enum_v<E>, Flags> operator&(const E& l, const E& r)
    {
        return FlagsCast(l) & FlagsCast(r);
    }

}

#endif//EXPLOSION_FLAGS_H
