//
// Created by johnk on 2023/7/8.
//

#pragma once

#include <Common/Math/Vector.h>

namespace Common {
    template <typename T>
    struct BoxBase {
        Vector<T, 3> min;
        Vector<T, 3> max;
    };

    template <typename T>
    struct Box : public BoxBase<T> {
        static inline Box FromMinExtent(T inMinX, T inMinY, T inMinZ, T inExtentX, T inExtentY, T inExtentZ);
        static inline Box FromMinExtent(Vector<T, 3> inMin, Vector<T, 3> inExtent);

        inline Box();
        inline Box(T inMinX, T inMinY, T inMinZ, T inMaxX, T inMaxY, T inMaxZ);
        inline Box(Vector<T, 3> inMin, Vector<T, 3> inMax);
        inline Box(const Box& inOther);
        inline Box(Box&& inOther) noexcept;
        inline Box& operator=(const Box& inOther);

        inline Vector<T, 3> Extent() const;
        inline T ExtentX() const;
        inline T ExtentY() const;
        inline T ExtentZ() const;
        inline Vector<T, 3> Center() const;
        inline T CenterX() const;
        inline T CenterY() const;
        inline T CenterZ() const;
        inline T Distance(const Box& inOther) const;
        // half of diagonal
        inline T Size() const;
        inline bool Inside(const Vector<T, 3>& inPoint) const;
        inline bool Intersect(const Box& inOther) const;

        template <typename IT>
        inline Box<IT> CastTo() const;
    };

    using IBox = Box<uint32_t>;
    using HBox = Box<HFloat>;
    using FBox = Box<float>;
    using DBox = Box<double>;
}

namespace Common {
    template <typename T>
    Box<T> Box<T>::FromMinExtent(T inMinX, T inMinY, T inMinZ, T inExtentX, T inExtentY, T inExtentZ)
    {
        return Box<T>(inMinX, inMinY, inMinZ, inMinX + inExtentX, inMinY + inExtentY, inMinZ + inExtentZ);
    }

    template <typename T>
    Box<T> Box<T>::FromMinExtent(Vector<T, 3> inMin, Vector<T, 3> inExtent)
    {
        return Box<T>(inMin, inMin + inExtent);
    }

    template <typename T>
    Box<T>::Box()
    {
        this->min = VecConsts<T, 3>::zero;
        this->max = VecConsts<T, 3>::zero;
    }

    template <typename T>
    Box<T>::Box(T inMinX, T inMinY, T inMinZ, T inMaxX, T inMaxY, T inMaxZ)
    {
        this->min = Vector<T, 3>(inMinX, inMinY, inMinZ);
        this->max = Vector<T, 3>(inMaxX, inMaxY, inMaxZ);
    }

    template <typename T>
    Box<T>::Box(Vector<T, 3> inMin, Vector<T, 3> inMax)
    {
        this->min = std::move(inMin);
        this->max = std::move(inMax);
    }

    template <typename T>
    Box<T>::Box(const Box<T>& inOther)
    {
        this->min = inOther.min;
        this->max = inOther.max;
    }

    template <typename T>
    Box<T>::Box(Box<T>&& inOther) noexcept
    {
        this->min = std::move(inOther.min);
        this->max = std::move(inOther.max);
    }

    template <typename T>
    Box<T>& Box<T>::operator=(const Box<T>& inOther)
    {
        this->min = inOther.min;
        this->max = inOther.max;
        return *this;
    }

    template <typename T>
    Vector<T, 3> Box<T>::Extent() const
    {
        return this->max - this->min;
    }

    template <typename T>
    T Box<T>::ExtentX() const
    {
        return this->max.x - this->min.x;
    }

    template <typename T>
    T Box<T>::ExtentY() const
    {
        return this->max.y - this->min.y;
    }

    template <typename T>
    T Box<T>::ExtentZ() const
    {
        return this->max.z - this->min.z;
    }

    template <typename T>
    Vector<T, 3> Box<T>::Center() const
    {
        return (this->min + this->max) / T(2);
    }

    template <typename T>
    T Box<T>::CenterX() const
    {
        return (this->min.x + this->max.x) / T(2);
    }

    template <typename T>
    T Box<T>::CenterY() const
    {
        return (this->min.y + this->max.y) / T(2);
    }

    template <typename T>
    T Box<T>::CenterZ() const
    {
        return (this->min.z + this->max.z) / T(2);
    }

    template <typename T>
    T Box<T>::Distance(const Box& inOther) const
    {
        Vector<T, 3> direction = Center() - inOther.Center();
        return direction.Model();
    }

    template <typename T>
    T Box<T>::Size() const
    {
        Vector<T, 3> diagonal = this->max - this->min;
        return diagonal.Model() / T(2);
    }

    template <typename T>
    bool Box<T>::Inside(const Vector<T, 3>& inPoint) const
    {
        return inPoint.x >= this->min.x && inPoint.x <= this->max.x
            && inPoint.y >= this->min.y && inPoint.y <= this->max.y
            && inPoint.z >= this->min.z && inPoint.z <= this->max.z;
    }

    template <typename T>
    bool Box<T>::Intersect(const Box& inOther) const
    {
        return this->min.x < inOther.max.x
            && this->min.y < inOther.max.y
            && this->min.z < inOther.max.z
            && inOther.min.x < this->max.x
            && inOther.min.y < this->max.y
            && inOther.min.z < this->max.z;
    }

    template <typename T>
    template <typename IT>
    Box<IT> Box<T>::CastTo() const
    {
        return Box<IT>(
            this->min.template CastTo<IT>(),
            this->max.template CastTo<IT>()
        );
    }
}
