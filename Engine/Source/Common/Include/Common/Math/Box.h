//
// Created by johnk on 2023/7/8.
//

#pragma once

#include <Common/Math/Vector.h>
#include <Common/Serialization.h>

namespace Common {
    template <typename T>
    struct BoxBase {
        Vec<T, 3> min;
        Vec<T, 3> max;
    };

    template <typename T>
    struct Box : BoxBase<T> {
        static Box FromMinExtent(T inMinX, T inMinY, T inMinZ, T inExtentX, T inExtentY, T inExtentZ);
        static Box FromMinExtent(Vec<T, 3> inMin, Vec<T, 3> inExtent);

        Box();
        Box(T inMinX, T inMinY, T inMinZ, T inMaxX, T inMaxY, T inMaxZ);
        Box(Vec<T, 3> inMin, Vec<T, 3> inMax);
        Box(const Box& inOther);
        Box(Box&& inOther) noexcept;
        Box& operator=(const Box& inOther);

        Vec<T, 3> Extent() const;
        T ExtentX() const;
        T ExtentY() const;
        T ExtentZ() const;
        Vec<T, 3> Center() const;
        T CenterX() const;
        T CenterY() const;
        T CenterZ() const;
        T Distance(const Box& inOther) const;
        // half of diagonal
        T Size() const;
        bool Inside(const Vec<T, 3>& inPoint) const;
        bool Intersect(const Box& inOther) const;

        template <typename IT>
        Box<IT> CastTo() const;
    };

    using IBox = Box<int32_t>;
    using UBox = Box<uint32_t>;
    using HBox = Box<HFloat>;
    using FBox = Box<float>;
    using DBox = Box<double>;
}

namespace Common { // NOLINT
    template <typename T>
    struct Serializer<Box<T>> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId
            = HashUtils::StrCrc32("Common::Box")
            + Serializer<T>::typeId;

        static void Serialize(SerializeStream& stream, const Common::Box<T>& value)
        {
            TypeIdSerializer<Common::Box<T>>::Serialize(stream);

            Serializer<Vec<T, 3>>::Serialize(stream, value.min);
            Serializer<Vec<T, 3>>::Serialize(stream, value.max);
        }

        static bool Deserialize(DeserializeStream& stream, Common::Box<T>& value)
        {
            if (!TypeIdSerializer<Common::Box<T>>::Deserialize(stream)) {
                return false;
            }

            Serializer<Vec<T, 3>>::Deserialize(stream, value.min);
            Serializer<Vec<T, 3>>::Deserialize(stream, value.max);
            return true;
        }
    };
}

namespace Common {
    template <typename T>
    Box<T> Box<T>::FromMinExtent(T inMinX, T inMinY, T inMinZ, T inExtentX, T inExtentY, T inExtentZ)
    {
        return Box(inMinX, inMinY, inMinZ, inMinX + inExtentX, inMinY + inExtentY, inMinZ + inExtentZ);
    }

    template <typename T>
    Box<T> Box<T>::FromMinExtent(Vec<T, 3> inMin, Vec<T, 3> inExtent)
    {
        return Box(inMin, inMin + inExtent);
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
        this->min = Vec<T, 3>(inMinX, inMinY, inMinZ);
        this->max = Vec<T, 3>(inMaxX, inMaxY, inMaxZ);
    }

    template <typename T>
    Box<T>::Box(Vec<T, 3> inMin, Vec<T, 3> inMax)
    {
        this->min = std::move(inMin);
        this->max = std::move(inMax);
    }

    template <typename T>
    Box<T>::Box(const Box& inOther)
    {
        this->min = inOther.min;
        this->max = inOther.max;
    }

    template <typename T>
    Box<T>::Box(Box&& inOther) noexcept
    {
        this->min = std::move(inOther.min);
        this->max = std::move(inOther.max);
    }

    template <typename T>
    Box<T>& Box<T>::operator=(const Box& inOther)
    {
        this->min = inOther.min;
        this->max = inOther.max;
        return *this;
    }

    template <typename T>
    Vec<T, 3> Box<T>::Extent() const
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
    Vec<T, 3> Box<T>::Center() const
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
        Vec<T, 3> direction = Center() - inOther.Center();
        return direction.Model();
    }

    template <typename T>
    T Box<T>::Size() const
    {
        Vec<T, 3> diagonal = this->max - this->min;
        return diagonal.Model() / T(2);
    }

    template <typename T>
    bool Box<T>::Inside(const Vec<T, 3>& inPoint) const
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
