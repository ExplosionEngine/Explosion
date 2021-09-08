//
// Created by John Kindem on 2021/9/5.
//

#ifndef EXPLOSION_MIRROR_EXCEPTION_H
#define EXPLOSION_MIRROR_EXCEPTION_H

#include <stdexcept>

namespace Explosion::Mirror {
    class BadAnyCastException : public std::exception {
    public:
        [[nodiscard]] const char* what() const noexcept override
        {
            return "bad any cast";
        }
    };

    class NoSuchKeyException : public std::exception {
    public:
        [[nodiscard]] const char * what() const noexcept override
        {
            return "no such key";
        }
    };
}

#endif //EXPLOSION_MIRROR_EXCEPTION_H
