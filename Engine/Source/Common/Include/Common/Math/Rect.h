//
// Created by johnk on 2023/7/8.
//

#pragma once

#include <Common/Math/Vector.h>

namespace Common {
    template <typename T>
    struct RectBase {
        Vector<T, 2> min;
        Vector<T, 2> max;
    };

    template <typename T>
    struct Rect : public RectBase<T> {
        static inline Rect FromMinExtent(T inMinX, T inMinY, T inExtentX, T inExtentY);
        static inline Rect FromMinExtent(Vector<T, 2> inMin, Vector<T, 2> inExtent);

        inline Rect();
        inline Rect(T inMinX, T inMinY, T inMaxX, T inMaxY);
        inline Rect(Vector<T, 2> inMin, Vector<T, 2> inMax);
        inline Rect(const Rect& inOther);
        inline Rect(Rect&& inOther) noexcept;
        inline Rect& operator=(const Rect& inOther);

        inline Vector<T, 2> Extent() const;
        inline T ExtentX() const;
        inline T ExtentY() const;
        inline Vector<T, 2> Center() const;
        inline T CenterX() const;
        inline T CenterY() const;
        inline T Distance(const Rect& inOther) const;
        // half of diagonal
        inline T Size() const;
        inline bool Inside(const Vector<T, 2>& inPoint) const;
        inline bool Intersect(const Rect& inOther) const;

        template <typename IT>
        inline Rect<IT> CastTo() const;
    };

    using IRect = Rect<int32_t>;
    using HRect = Rect<HFloat>;
    using FRect = Rect<float>;
    using DRect = Rect<double>;
}

namespace Common {
    template <typename T>
    Rect<T> Rect<T>::FromMinExtent(T inMinX, T inMinY, T inExtentX, T inExtentY)
    {
        return Rect<T>(inMinX, inMinY, inMinX + inExtentX, inMinY + inExtentY);
    }

    template <typename T>
    Rect<T> Rect<T>::FromMinExtent(Vector<T, 2> inMin, Vector<T, 2> inExtent)
    {
        return Rect<T>(inMin, inMin + inExtent);
    }

    template <typename T>
    Rect<T>::Rect()
    {
        this->min = VecConsts<T, 2>::zero;
        this->maxx = VecConsts<T, 3>::zero;
    }

    template <typename T>
    Rect<T>::Rect(T inMinX, T inMinY, T inMaxX, T inMaxY)
    {
        this->min = Vector<T, 2>(inMinX, inMinY);
        this->max = Vector<T, 2>(inMaxX, inMaxY);
    }

    template <typename T>
    Rect<T>::Rect(Vector<T, 2> inMin, Vector<T, 2> inMax)
    {
        this->min = std::move(inMin);
        this->max = std::move(inMax);
    }

    template <typename T>
    Rect<T>::Rect(const Rect<T>& inOther)
    {
        this->min = inOther.min;
        this->max = inOther.max;
    }

    template <typename T>
    Rect<T>::Rect(Rect<T>&& inOther) noexcept
    {
        this->min = std::move(inOther.min);
        this->max = std::move(inOther.max);
    }

    template <typename T>
    Rect<T>& Rect<T>::operator=(const Rect<T>& inOther)
    {
        this->min = inOther.min;
        this->max = inOther.max;
        return *this;
    }

    template <typename T>
    Vector<T, 2> Rect<T>::Extent() const
    {
        return this->max - this->min;
    }

    template <typename T>
    T Rect<T>::ExtentX() const
    {
        return this->max.x - this->min.x;
    }

    template <typename T>
    T Rect<T>::ExtentY() const
    {
        return this->max.y - this->min.y;
    }

    template <typename T>
    Vector<T, 2> Rect<T>::Center() const
    {
        return (this->min + this->max) / T(2);
    }

    template <typename T>
    T Rect<T>::CenterX() const
    {
        return (this->min.x + this->max.x) / T(2);
    }

    template <typename T>
    T Rect<T>::CenterY() const
    {
        return (this->min.y + this->max.y) / T(2);
    }

    template <typename T>
    T Rect<T>::Distance(const Rect& inOther) const
    {
        Vector<T, 2> direction = Center() - inOther.Center();
        return direction.Model();
    }

    template <typename T>
    T Rect<T>::Size() const
    {
        Vector<T, 3> diagonal = this->max - this->min;
        return diagonal.Model() / T(2);
    }

    template <typename T>
    bool Rect<T>::Inside(const Vector<T, 2>& inPoint) const
    {
        return inPoint.x >= this->min.x && inPoint.x <= this->max.x
            && inPoint.y >= this->min.y && inPoint.y <= this->max.y
            && inPoint.z >= this->min.z && inPoint.z <= this->max.z;
    }

    template <typename T>
    bool Rect<T>::Intersect(const Rect& inOther) const
    {
        // TODO
        return false;
    }

    template <typename T>
    template <typename IT>
    Rect<IT> Rect<T>::CastTo() const
    {
        return Rect<IT>(
            this->min.template CastTo<IT>(),
            this->max.template CastTo<IT>()
        );
    }
}
