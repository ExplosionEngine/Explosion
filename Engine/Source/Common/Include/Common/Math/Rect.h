//
// Created by johnk on 2023/7/8.
//

#pragma once

#include <Common/Math/Vector.h>
#include <Common/Serialization.h>

namespace Common {
    template <typename T>
    struct RectBase {
        Vec<T, 2> min;
        Vec<T, 2> max;
    };

    template <typename T>
    struct Rect : RectBase<T> {
        static Rect FromMinExtent(T inMinX, T inMinY, T inExtentX, T inExtentY);
        static Rect FromMinExtent(Vec<T, 2> inMin, Vec<T, 2> inExtent);

        Rect();
        Rect(T inMinX, T inMinY, T inMaxX, T inMaxY);
        Rect(Vec<T, 2> inMin, Vec<T, 2> inMax);
        Rect(const Rect& inOther);
        Rect(Rect&& inOther) noexcept;
        Rect& operator=(const Rect& inOther);

        Vec<T, 2> Extent() const;
        T ExtentX() const;
        T ExtentY() const;
        Vec<T, 2> Center() const;
        T CenterX() const;
        T CenterY() const;
        T Distance(const Rect& inOther) const;
        // half of diagonal
        T Size() const;
        bool Inside(const Vec<T, 2>& inPoint) const;
        bool Intersect(const Rect& inOther) const;

        template <typename IT>
        Rect<IT> CastTo() const;
    };

    using IRect = Rect<int32_t>;
    using URect = Rect<uint32_t>;
    using HRect = Rect<HFloat>;
    using FRect = Rect<float>;
    using DRect = Rect<double>;
}

namespace Common {
    template <typename T>
    struct Serializer<Rect<T>> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId
            = HashUtils::StrCrc32("Common::Rect")
            + Serializer<T>::typeId;

        static void Serialize(SerializeStream& stream, const Rect<T>& value)
        {
            TypeIdSerializer<Rect<T>>::Serialize(stream);

            Serializer<Vec<T, 2>>::Serialize(stream, value.min);
            Serializer<Vec<T, 2>>::Serialize(stream, value.max);
        }

        static bool Deserialize(DeserializeStream& stream, Rect<T>& value)
        {
            if (!TypeIdSerializer<Rect<T>>::Deserialize(stream)) {
                return false;
            }

            Serializer<Vec<T, 2>>::Deserialize(stream, value.min);
            Serializer<Vec<T, 2>>::Deserialize(stream, value.max);
            return true;
        }
    };
}

namespace Common {
    template <typename T>
    Rect<T> Rect<T>::FromMinExtent(T inMinX, T inMinY, T inExtentX, T inExtentY)
    {
        return Rect(inMinX, inMinY, inMinX + inExtentX, inMinY + inExtentY);
    }

    template <typename T>
    Rect<T> Rect<T>::FromMinExtent(Vec<T, 2> inMin, Vec<T, 2> inExtent)
    {
        return Rect(inMin, inMin + inExtent);
    }

    template <typename T>
    Rect<T>::Rect()
    {
        this->min = VecConsts<T, 2>::zero;
        this->max = VecConsts<T, 2>::zero;
    }

    template <typename T>
    Rect<T>::Rect(T inMinX, T inMinY, T inMaxX, T inMaxY)
    {
        this->min = Vec<T, 2>(inMinX, inMinY);
        this->max = Vec<T, 2>(inMaxX, inMaxY);
    }

    template <typename T>
    Rect<T>::Rect(Vec<T, 2> inMin, Vec<T, 2> inMax)
    {
        this->min = std::move(inMin);
        this->max = std::move(inMax);
    }

    template <typename T>
    Rect<T>::Rect(const Rect& inOther)
    {
        this->min = inOther.min;
        this->max = inOther.max;
    }

    template <typename T>
    Rect<T>::Rect(Rect&& inOther) noexcept
    {
        this->min = std::move(inOther.min);
        this->max = std::move(inOther.max);
    }

    template <typename T>
    Rect<T>& Rect<T>::operator=(const Rect& inOther)
    {
        this->min = inOther.min;
        this->max = inOther.max;
        return *this;
    }

    template <typename T>
    Vec<T, 2> Rect<T>::Extent() const
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
    Vec<T, 2> Rect<T>::Center() const
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
        Vec<T, 2> direction = Center() - inOther.Center();
        return direction.Model();
    }

    template <typename T>
    T Rect<T>::Size() const
    {
        Vec<T, 3> diagonal = this->max - this->min;
        return diagonal.Model() / T(2);
    }

    template <typename T>
    bool Rect<T>::Inside(const Vec<T, 2>& inPoint) const
    {
        return inPoint.x >= this->min.x && inPoint.x <= this->max.x
            && inPoint.y >= this->min.y && inPoint.y <= this->max.y;
    }

    template <typename T>
    bool Rect<T>::Intersect(const Rect& inOther) const
    {
        return this->min.x < inOther.max.x
            && this->min.y < inOther.max.y
            && inOther.min.x < this->max.x
            && inOther.min.y < this->max.y;
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
