//
// Created by LiZhen on 2021/8/24.
//

#ifndef EXPLOSION_TYPETRAITS_H
#define EXPLOSION_TYPETRAITS_H

namespace Explosion {

    template <typename T>
    struct Counter {
        void Inc() { ++t; }
        void Dec() { --t; }
        T t;
    };

}

#endif//EXPLOSION_TYPETRAITS_H
