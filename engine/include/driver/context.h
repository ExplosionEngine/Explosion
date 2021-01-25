//
// Created by John Kindem on 2021/1/9 0009.
//

#ifndef EXPLOSION_CONTEXT_H
#define EXPLOSION_CONTEXT_H

namespace Explosion {
    class Context {
    public:
        virtual ~Context() = 0;

    protected:
        Context() = default;

    private:
    };
}

#endif //EXPLOSION_CONTEXT_H
