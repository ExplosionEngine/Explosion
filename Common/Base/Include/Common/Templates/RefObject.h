//
// Created by Zach Lee on 2021/5/9.
//

#ifndef EXPLOSION_REF_OBJECT_H
#define EXPLOSION_REF_OBJECT_H

#include <type_traits>
#include <atomic>

#include <Common/Exception.h>

namespace Explosion {
    template<typename T>
    struct CounterTraits {
        uint32_t operator()(const T &t) {
            return static_cast<uint32_t>(t);
        }
    };

    template<>
    struct CounterTraits<std::atomic_uint32_t> {
        uint32_t operator()(const std::atomic_uint32_t &t) {
            return t.load();
        }
    };

    template<typename T, typename Counter, template<typename U> class CTraits = CounterTraits>
    class RefObject {
    public:
        using ObjType = T;
        using CounterType = Counter;
        using TraitType = CTraits<Counter>;

        RefObject() : counter{} {}

        virtual ~RefObject() {}

        virtual void AddRef() {
            counter++;
        }

        virtual void RemoveRef() {
            if (counter != 0) counter--;
        }

        uint32_t GetRef() const {
            return TraitType()(counter);
        }

    private:
        CounterType counter;
    };

    template<typename T>
    using URefObject = RefObject<T, uint32_t>;

    template<typename T>
    using UTsRefObject = RefObject<T, std::atomic_uint32_t>;

    template<typename T>
    class CounterPtrBase {
    public:
        CounterPtrBase(T *p) : ptr(p) {
            if (ptr != nullptr) {
                ptr->AddRef();
            }
        }

        virtual ~CounterPtrBase() {
            if (ptr != nullptr) {
                ptr->RemoveRef();
            }
        }

        CounterPtrBase(const CounterPtrBase &p) { UpdateRefs(p); }

        CounterPtrBase &operator=(const CounterPtrBase &p) {
            UpdateRefs(p);
            return *this;
        }

        T *Get() {
            return ptr;
        }

    protected:
        void UpdateRefs(const CounterPtrBase &p) {
            if (ptr != nullptr) {
                ptr->RemoveRef();
            }
            ptr = p.ptr;
            if (ptr != nullptr) {
                ptr->AddRefs();
            }
        }

        T *ptr;
    };

    template<typename T>
    class CounterPtr : public CounterPtrBase<typename T::ObjType> {
    public:
        CounterPtr() : CounterPtrBase<typename T::ObjType>(nullptr) {}

        template<typename U>
        CounterPtr(U *p) : CounterPtrBase<typename T::ObjType>(p) {}

        template<typename U>
        CounterPtr(CounterPtr<U> &u) : CounterPtrBase<typename T::ObjType>(u.Get()) {
            static_assert(std::is_base_of_v<T, U>, "U must derived from T");
        }

        template<typename U>
        CounterPtr &operator=(const CounterPtr<U> &u) {
            static_assert(std::is_base_of_v<T, U>, "U must derived from T");
            CounterPtrBase<typename T::ObjType>::operator=(u);
            return *this;
        }

        ~CounterPtr() {}

        operator bool() { return CounterPtrBase<typename T::ObjType>::ptr != nullptr; }

        T *operator->() { return static_cast<T *>(CounterPtrBase<typename T::ObjType>::ptr); }
    };
}

#endif //EXPLOSION_REFOBJECT_H
