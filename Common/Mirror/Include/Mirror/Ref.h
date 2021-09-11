//
// Created by johnk on 2021/9/4.
//

#ifndef EXPLOSION_REF_H
#define EXPLOSION_REF_H

#include <Mirror/Any.h>

namespace Explosion::Mirror {
    class Ref {
    public:
        explicit Ref(void* instance) : instance(instance) {}
        explicit Ref(Any& any) : instance(any.RawValue()) {}
        Ref(Ref& ref) = default;
        Ref(Ref&& ref)  noexcept : instance(ref.instance) {}
        ~Ref() = default;
        Ref& operator=(const Ref& ref) = default;

        [[nodiscard]] void* Value() const
        {
            return instance;
        }

    private:
        void* instance;
    };
}

#endif //EXPLOSION_REF_H
