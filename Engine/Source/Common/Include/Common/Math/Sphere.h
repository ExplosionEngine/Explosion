//
// Created by johnk on 2023/7/8.
//

#pragma once

#include <Common/Math/Vector.h>
#include <Common/Serialization.h>

namespace Common {
    template <typename T>
    struct SphereBase {
        Vector<T, 3> center;
        T radius;
    };

    template <typename T>
    struct Sphere : public SphereBase<T> {
        inline Sphere();
        inline Sphere(T inX, T inY, T inZ, T inRadius);
        inline Sphere(Vector<T, 3> inCenter, T inRadius);
        inline Sphere(const Sphere& inOther);
        inline Sphere(Sphere&& inOther) noexcept;
        inline Sphere& operator=(const Sphere& inOther);

        inline T Distance(const Sphere& inOther) const;
        inline bool Inside(const Vector<T, 3>& inPoint) const;
        inline bool Intersect(const Sphere& inOther) const;

        template <typename IT>
        inline Sphere<IT> CastTo() const;
    };

    using ISphere = Sphere<int32_t>;
    using HSphere = Sphere<HFloat>;
    using FSphere = Sphere<float>;
    using DSphere = Sphere<double>;
}

namespace Common {
    template <typename T>
    struct Serializer<Sphere<T>> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId
            = Common::HashUtils::StrCrc32("Common::Sphere")
              + Serializer<T>::typeId;

        static void Serialize(SerializeStream& stream, const Sphere<T>& value)
        {
            TypeIdSerializer<Sphere<T>>::Serialize(stream);

            Serializer<Vector<T, 3>>::Serialize(stream, value.center);
            Serializer<T>::Serialize(stream, value.radius);
        }

        static bool Deserialize(DeserializeStream& stream, Sphere<T>& value)
        {
            if (!TypeIdSerializer<Sphere<T>>::Deserialize(stream)) {
                return false;
            }

            Serializer<Vector<T, 3>>::Deserialize(stream, value.center);
            Serializer<T>::Deserialize(stream, value.radius);
        }
    };
}

namespace Common {
    template <typename T>
    Sphere<T>::Sphere()
    {
        this->center = VecConsts<T, 3>::zero;
        this->radius = 0;
    }

    template <typename T>
    Sphere<T>::Sphere(T inX, T inY, T inZ, T inRadius)
    {
        this->center = Vector<T, 3>(inX, inY, inZ);
        this->radius = inRadius;
    }

    template <typename T>
    Sphere<T>::Sphere(Vector<T, 3> inCenter, T inRadius)
    {
        this->center = std::move(inCenter);
        this->radius = inRadius;
    }

    template <typename T>
    Sphere<T>::Sphere(const Sphere<T>& inOther)
    {
        this->center = inOther.center;
        this->radius = inOther.radius;
    }

    template <typename T>
    Sphere<T>::Sphere(Sphere<T>&& inOther) noexcept
    {
        this->center = std::move(inOther.center);
        this->radius = inOther.radius;
    }

    template <typename T>
    Sphere<T>& Sphere<T>::operator=(const Sphere<T>& inOther)
    {
        this->center = inOther.center;
        this->radius = inOther.radius;
        return *this;
    }

    template <typename T>
    T Sphere<T>::Distance(const Sphere& inOther) const
    {
        Vector<T, 3> direction = this->center - inOther.center;
        return direction.Model();
    }

    template <typename T>
    bool Sphere<T>::Inside(const Vector<T, 3>& inPoint) const
    {
        Vector<T, 3> direction = this->center - inPoint;
        return direction.Model() <= this->radius;
    }

    template <typename T>
    bool Sphere<T>::Intersect(const Sphere& inOther) const
    {
        Vector<T, 3> direction = this->center - inOther.center;
        return direction.Model() <= (this->radius + inOther.radius);
    }

    template <typename T>
    template <typename IT>
    Sphere<IT> Sphere<T>::CastTo() const
    {
        return Sphere<IT>(
            this->center.template CastTo<IT>(),
            static_cast<IT>(this->radius)
        );
    }
}
