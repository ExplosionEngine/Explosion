//
// Created by johnk on 2023/6/4.
//

#pragma once

#include <Common/Math/Simd.h>
#include <Common/Math/Half.h>
#include <Common/Math/Matrix.h>
#include <Common/Serialization.h>
#include <Common/String.h>

namespace Common {
    template <typename T> struct Angle;
    template <typename T> struct Radian;

    template <FloatingPoint T>
    struct AngleBase {
        T value;
    };

    template <FloatingPoint T>
    struct RadianBase {
        T value;
    };

    template <FloatingPoint T>
    struct QuaternionBase {
        T x;
        T y;
        T z;
        T w;
    };

    template <typename T>
    struct Angle : AngleBase<T> {
        Angle();
        explicit Angle(T inValue);
        explicit Angle(const Radian<T>& inValue);
        Angle(const Angle& inValue);
        Angle(Angle&& inValue) noexcept;
        Angle& operator=(const Angle& inValue);
        Angle& operator=(const Radian<T>& inValue);
        bool operator==(const Angle& inRhs) const;
        T ToRadian() const;
    };

    template <typename T>
    struct Radian : RadianBase<T> {
        Radian();
        explicit Radian(T inValue);
        explicit Radian(const Angle<T>& inValue);
        Radian(const Radian& inValue);
        Radian(Radian&& inValue) noexcept;
        Radian& operator=(const Radian& inValue);
        Radian& operator=(const Angle<T>& inValue);
        bool operator==(const Radian& inRhs) const;
        T ToAngle() const;
    };

    // left-hand coordinates system
    // +x -> from screen outer to inner
    // +y -> from left to right
    // +z -> from bttom to up
    template <typename T, MathBackend B = MathBackend::defaultBackend>
    struct Quaternion : QuaternionBase<T> {
        static Quaternion FromEulerZYX(T inAngleX, T inAngleY, T inAngleZ);
        static Quaternion FromEulerZYX(const Radian<T>& inRadianX, const Radian<T>& inRadianY, const Radian<T>& inRadianZ);

        Quaternion();
        Quaternion(T inW, T inX, T inY, T inZ);
        Quaternion(const Vec<T, 3, B>& inAxis, float inAngle);
        Quaternion(const Vec<T, 3, B>& inAxis, const Radian<T>& inRadian);
        Quaternion(const Quaternion& inValue) = default;
        Quaternion(Quaternion&& inValue) noexcept = default;
        Quaternion& operator=(const Quaternion& inValue) = default;

        bool operator==(const Quaternion& rhs) const;
        bool operator!=(const Quaternion& rhs) const;
        Quaternion operator+(const Quaternion& rhs) const;
        Quaternion operator-(const Quaternion& rhs) const;
        Quaternion operator*(T rhs) const;
        Quaternion operator*(const Quaternion& rhs) const;
        Quaternion operator/(T rhs) const;

        Quaternion& operator+=(const Quaternion& rhs);
        Quaternion& operator-=(const Quaternion& rhs);
        Quaternion& operator*=(T rhs);
        Quaternion& operator*=(const Quaternion& rhs);
        Quaternion& operator/=(T rhs);

        Vec<T, 3, B> ImaginaryPart() const;
        T Model() const;
        Quaternion Negatived() const;
        Quaternion Conjugated() const;
        Quaternion Normalized() const;
        T Dot(const Quaternion& rhs) const;
        // when axis faced to us, ccw as positive direction
        Vec<T, 3, B> RotateVector(const Vec<T, 3, B>& inVector) const;
        Mat<T, 4, 4, B> GetRotationMatrix() const;

        template <typename IT>
        Quaternion<IT, B> CastTo() const;
    };

    template <typename T, MathBackend B = MathBackend::defaultBackend>
    struct QuatConsts {
        static const Quaternion<T, B> zero;
        static const Quaternion<T, B> identity;
    };

    using HAngle = Angle<HFloat>;
    using FAngle = Angle<float>;
    using DAngle = Angle<double>;

    using HRadian = Radian<HFloat>;
    using FRadian = Radian<float>;
    using DRadian = Radian<double>;

    using HQuat = Quaternion<HFloat>;
    using FQuat = Quaternion<float>;
    using DQuat = Quaternion<double>;

    using HQuatConsts = QuatConsts<HFloat>;
    using FQuatConsts = QuatConsts<float>;
    using DQuatConsts = QuatConsts<double>;
}

namespace Common {
    template <Serializable T>
    struct Serializer<Angle<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("Common::Angle")
            + Serializer<T>::typeId;

        static size_t Serialize(BinarySerializeStream& stream, const Angle<T>& value)
        {
            return Serializer<T>::Serialize(stream, value.value);
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, Angle<T>& value)
        {
            return Serializer<T>::Deserialize(stream, value.value);
        }
    };

    template <Serializable T>
    struct Serializer<Radian<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("Common::Radian")
            + Serializer<T>::typeId;

        static size_t Serialize(BinarySerializeStream& stream, const Radian<T>& value)
        {
            return Serializer<T>::Serialize(stream, value.value);
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, Radian<T>& value)
        {
            return Serializer<T>::Deserialize(stream, value.value);
        }
    };

    template <Serializable T, MathBackend B>
    struct Serializer<Quaternion<T, B>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("Common::Quaternion")
            + Serializer<T>::typeId;

        static size_t Serialize(BinarySerializeStream& stream, const Quaternion<T, B>& value)
        {
            size_t serialized = 0;
            serialized += Serializer<T>::Serialize(stream, value.w);
            serialized += Serializer<T>::Serialize(stream, value.x);
            serialized += Serializer<T>::Serialize(stream, value.y);
            serialized += Serializer<T>::Serialize(stream, value.z);
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, Quaternion<T, B>& value)
        {
            size_t deserialized = 0;
            deserialized += Serializer<T>::Deserialize(stream, value.w);
            deserialized += Serializer<T>::Deserialize(stream, value.x);
            deserialized += Serializer<T>::Deserialize(stream, value.y);
            deserialized += Serializer<T>::Deserialize(stream, value.z);
            return deserialized;
        }
    };

    template <StringConvertible T>
    struct StringConverter<Angle<T>> {
        static std::string ToString(const Angle<T>& inValue)
        {
            return std::format("a{}", StringConverter<T>::ToString(inValue.value));
        }
    };

    template <StringConvertible T>
    struct StringConverter<Radian<T>> {
        static std::string ToString(const Radian<T>& inValue)
        {
            return StringConverter<T>::ToString(inValue.value);
        }
    };

    template <StringConvertible T, MathBackend B>
    struct StringConverter<Quaternion<T, B>> {
        static std::string ToString(const Quaternion<T, B>& inValue)
        {
            return std::format(
                "({}, {}, {}, {})",
                StringConverter<T>::ToString(inValue.w),
                StringConverter<T>::ToString(inValue.x),
                StringConverter<T>::ToString(inValue.y),
                StringConverter<T>::ToString(inValue.z));
        }
    };

    template <JsonSerializable T>
    struct JsonSerializer<Angle<T>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Angle<T>& inValue)
        {
            JsonSerializer<T>::JsonSerialize(outJsonValue, inAllocator, inValue.value);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, Angle<T>& outValue)
        {
            JsonSerializer<T>::JsonDeserialize(inJsonValue, outValue.value);
        }
    };

    template <JsonSerializable T>
    struct JsonSerializer<Radian<T>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Radian<T>& inValue)
        {
            JsonSerializer<T>::JsonSerialize(outJsonValue, inAllocator, inValue.value);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, Radian<T>& outValue)
        {
            JsonSerializer<T>::JsonDeserialize(inJsonValue, outValue.value);
        }
    };

    template <JsonSerializable T, MathBackend B>
    struct JsonSerializer<Quaternion<T, B>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Quaternion<T, B>& inValue)
        {
            rapidjson::Value xJson;
            JsonSerializer<T>::JsonSerialize(xJson, inAllocator, inValue.w);

            rapidjson::Value yJson;
            JsonSerializer<T>::JsonSerialize(yJson, inAllocator, inValue.x);

            rapidjson::Value zJson;
            JsonSerializer<T>::JsonSerialize(zJson, inAllocator, inValue.y);

            rapidjson::Value wJson;
            JsonSerializer<T>::JsonSerialize(wJson, inAllocator, inValue.z);

            outJsonValue.SetArray();
            outJsonValue.PushBack(xJson, inAllocator);
            outJsonValue.PushBack(yJson, inAllocator);
            outJsonValue.PushBack(zJson, inAllocator);
            outJsonValue.PushBack(wJson, inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, Quaternion<T, B>& outValue)
        {
            if (!inJsonValue.IsArray() || inJsonValue.Size() != 4) {
                return;
            }
            JsonSerializer<T>::JsonDeserialize(inJsonValue[0], outValue.w);
            JsonSerializer<T>::JsonDeserialize(inJsonValue[1], outValue.x);
            JsonSerializer<T>::JsonDeserialize(inJsonValue[2], outValue.y);
            JsonSerializer<T>::JsonDeserialize(inJsonValue[3], outValue.z);
        }
    };
}

namespace Common::Internal {
    // Per-backend dispatch for quaternion arithmetic. The primary template is the scalar implementation, so any
    // (T, B) without a specialization degrades gracefully to scalar; the SIMD specialization follows immediately after.
    template <typename T, MathBackend B>
    struct QuatOps {
        using Q = Quaternion<T, B>;

        static Q Add(const Q& a, const Q& b)
        {
            Q result;
            result.w = a.w + b.w;
            result.x = a.x + b.x;
            result.y = a.y + b.y;
            result.z = a.z + b.z;
            return result;
        }

        static Q Sub(const Q& a, const Q& b)
        {
            Q result;
            result.w = a.w - b.w;
            result.x = a.x - b.x;
            result.y = a.y - b.y;
            result.z = a.z - b.z;
            return result;
        }

        static Q MulScalar(const Q& a, T b)
        {
            Q result;
            result.w = a.w * b;
            result.x = a.x * b;
            result.y = a.y * b;
            result.z = a.z * b;
            return result;
        }

        static Q DivScalar(const Q& a, T b)
        {
            Q result;
            result.w = a.w / b;
            result.x = a.x / b;
            result.y = a.y / b;
            result.z = a.z / b;
            return result;
        }

        static Q Mul(const Q& a, const Q& b)
        {
            Q result;
            result.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
            result.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
            result.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
            result.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
            return result;
        }

        static T Dot(const Q& a, const Q& b)
        {
            return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
        }
    };

    // QuaternionBase<float> stores x, y, z, w as four contiguous floats, so &q.x is the base of a 16-byte block that
    // maps to an unaligned 128-bit load/store. The element-wise ops and the dot product map onto the F32x4 wrapper
    // directly; the Hamilton product is expressed as four broadcast-and-permute terms (see Mul).
    template <>
    struct QuatOps<float, MathBackend::simd> {
        using Q = Quaternion<float, MathBackend::simd>;

        static Q Add(const Q& a, const Q& b) { Q r; Simd::MapBinary<4>(&r.x, &a.x, &b.x, Simd::AddOp {}); return r; }
        static Q Sub(const Q& a, const Q& b) { Q r; Simd::MapBinary<4>(&r.x, &a.x, &b.x, Simd::SubOp {}); return r; }

        static Q MulScalar(const Q& a, float b) { Q r; Simd::MapScalar<4>(&r.x, &a.x, b, Simd::MulOp {}); return r; }
        static Q DivScalar(const Q& a, float b) { Q r; Simd::MapScalar<4>(&r.x, &a.x, b, Simd::DivOp {}); return r; }

        // Hamilton product, with both quaternions loaded as (x, y, z, w). Each row of the product is one component of
        // a broadcast against a sign-flipped permutation of b, summed across the four components of a:
        //   result = aw*(bx,by,bz,bw) + ax*(bw,-bz,by,-bx) + ay*(bz,bw,-bx,-by) + az*(-by,bx,bw,-bz)
        // The accumulation order matches the scalar reference above, so both backends produce identical results.
        static Q Mul(const Q& a, const Q& b)
        {
            const Simd::F32x4 av = Simd::LoadU(&a.x);
            const Simd::F32x4 bv = Simd::LoadU(&b.x);

            const Simd::F32x4 sign0 = Simd::Set(1.0f, -1.0f, 1.0f, -1.0f);
            const Simd::F32x4 sign1 = Simd::Set(1.0f, 1.0f, -1.0f, -1.0f);
            const Simd::F32x4 sign2 = Simd::Set(-1.0f, 1.0f, 1.0f, -1.0f);

            Simd::F32x4 acc = Simd::Mul(Simd::Splat<3>(av), bv);
            acc = Simd::Add(acc, Simd::Mul(Simd::Splat<0>(av), Simd::Mul(Simd::Shuffle<3, 2, 1, 0>(bv), sign0)));
            acc = Simd::Add(acc, Simd::Mul(Simd::Splat<1>(av), Simd::Mul(Simd::Shuffle<2, 3, 0, 1>(bv), sign1)));
            acc = Simd::Add(acc, Simd::Mul(Simd::Splat<2>(av), Simd::Mul(Simd::Shuffle<1, 0, 3, 2>(bv), sign2)));

            Q result;
            Simd::StoreU(&result.x, acc);
            return result;
        }

        static float Dot(const Q& a, const Q& b)
        {
            return Simd::Sum(Simd::Mul(Simd::LoadU(&a.x), Simd::LoadU(&b.x)));
        }
    };
}

namespace Common {
    template <typename T>
    Angle<T>::Angle()
    {
        this->value = 0;
    }

    template <typename T>
    Angle<T>::Angle(T inValue)
    {
        this->value = inValue;
    }

    template <typename T>
    Angle<T>::Angle(const Radian<T>& inValue) : Angle(inValue.ToAngle()) {}

    template <typename T>
    Angle<T>::Angle(const Angle& inValue) : Angle(inValue.value) {}

    template <typename T>
    Angle<T>::Angle(Angle&& inValue) noexcept : Angle(inValue.value) {}

    template <typename T>
    Angle<T>& Angle<T>::operator=(const Angle& inValue)
    {
        this->value = inValue.value;
        return *this;
    }

    template <typename T>
    Angle<T>& Angle<T>::operator=(const Radian<T>& inValue)
    {
        this->value = inValue.value;
        return *this;
    }

    template <typename T>
    bool Angle<T>::operator==(const Angle& inRhs) const
    {
        return CompareNumber(this->value, inRhs.value);
    }

    template <typename T>
    T Angle<T>::ToRadian() const
    {
        return this->value / 180.0f * pi;
    }

    template <typename T>
    Radian<T>::Radian()
    {
        this->value = 0;
    }

    template <typename T>
    Radian<T>::Radian(T inValue)
    {
        this->value = inValue;
    }

    template <typename T>
    Radian<T>::Radian(const Angle<T>& inValue) : Radian(inValue.ToRadian()) {}

    template <typename T>
    Radian<T>::Radian(const Radian& inValue) : Radian(inValue.value) {}

    template <typename T>
    Radian<T>::Radian(Radian&& inValue) noexcept : Radian(inValue.value) {}

    template <typename T>
    Radian<T>& Radian<T>::operator=(const Radian& inValue)
    {
        this->value = inValue.value;
        return *this;
    }

    template <typename T>
    Radian<T>& Radian<T>::operator=(const Angle<T>& inValue)
    {
        this->value = inValue.value;
        return *this;
    }

    template <typename T>
    bool Radian<T>::operator==(const Radian& inRhs) const
    {
        return CompareNumber(this->value, inRhs.value);
    }

    template <typename T>
    T Radian<T>::ToAngle() const
    {
        return this->value * 180.0f / pi;
    }

    template <typename T, MathBackend B>
    const Quaternion<T, B> QuatConsts<T, B>::zero = Quaternion<T, B>();

    template <typename T, MathBackend B>
    const Quaternion<T, B> QuatConsts<T, B>::identity = Quaternion<T, B>(1, 0, 0, 0);

    template <typename T, MathBackend B>
    Quaternion<T, B> Quaternion<T, B>::FromEulerZYX(T inAngleX, T inAngleY, T inAngleZ)
    {
        return Quaternion(VecConsts<T, 3, B>::unitZ, inAngleZ)
            * Quaternion(VecConsts<T, 3, B>::unitY, inAngleY)
            * Quaternion(VecConsts<T, 3, B>::unitX, inAngleX);
    }

    template <typename T, MathBackend B>
    Quaternion<T, B> Quaternion<T, B>::FromEulerZYX(const Radian<T>& inRadianX, const Radian<T>& inRadianY, const Radian<T>& inRadianZ)
    {
        return FromEulerZYX(inRadianX.ToAngle(), inRadianY.ToAngle(), inRadianZ.ToAngle());
    }

    template <typename T, MathBackend B>
    Quaternion<T, B>::Quaternion()
    {
        this->w = 0;
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    template <typename T, MathBackend B>
    Quaternion<T, B>::Quaternion(T inW, T inX, T inY, T inZ)
    {
        this->w = inW;
        this->x = inX;
        this->y = inY;
        this->z = inZ;
    }

    template <typename T, MathBackend B>
    Quaternion<T, B>::Quaternion(const Vec<T, 3, B>& inAxis, float inAngle)
    {
        Vec<T, 3, B> axis = inAxis.Normalized();
        T halfRadian = Angle<T>(inAngle).ToRadian() / 2.0f;
        T halfRadianSin = std::sin(halfRadian);
        T halfRadianCos = std::cos(halfRadian);

        this->w = halfRadianCos;
        this->x = axis.x * halfRadianSin;
        this->y = axis.y * halfRadianSin;
        this->z = axis.z * halfRadianSin;
    }

    template <typename T, MathBackend B>
    Quaternion<T, B>::Quaternion(const Vec<T, 3, B>& inAxis, const Radian<T>& inRadian)
        : Quaternion(inAxis, inRadian.ToAngle())
    {
    }

    template <typename T, MathBackend B>
    bool Quaternion<T, B>::operator==(const Quaternion& rhs) const
    {
        return CompareNumber(this->w, rhs.w)
            && CompareNumber(this->x, rhs.x)
            && CompareNumber(this->y, rhs.y)
            && CompareNumber(this->z, rhs.z);
    }

    template <typename T, MathBackend B>
    bool Quaternion<T, B>::operator!=(const Quaternion& rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T, MathBackend B>
    Quaternion<T, B> Quaternion<T, B>::operator+(const Quaternion& rhs) const
    {
        return Internal::QuatOps<T, B>::Add(*this, rhs);
    }

    template <typename T, MathBackend B>
    Quaternion<T, B> Quaternion<T, B>::operator-(const Quaternion& rhs) const
    {
        return Internal::QuatOps<T, B>::Sub(*this, rhs);
    }

    template <typename T, MathBackend B>
    Quaternion<T, B> Quaternion<T, B>::operator*(T rhs) const
    {
        return Internal::QuatOps<T, B>::MulScalar(*this, rhs);
    }

    template <typename T, MathBackend B>
    Quaternion<T, B> Quaternion<T, B>::operator*(const Quaternion& rhs) const
    {
        return Internal::QuatOps<T, B>::Mul(*this, rhs);
    }

    template <typename T, MathBackend B>
    Quaternion<T, B> Quaternion<T, B>::operator/(T rhs) const
    {
        return Internal::QuatOps<T, B>::DivScalar(*this, rhs);
    }

    template <typename T, MathBackend B>
    Quaternion<T, B>& Quaternion<T, B>::operator+=(const Quaternion& rhs)
    {
        *this = Internal::QuatOps<T, B>::Add(*this, rhs);
        return *this;
    }

    template <typename T, MathBackend B>
    Quaternion<T, B>& Quaternion<T, B>::operator-=(const Quaternion& rhs)
    {
        *this = Internal::QuatOps<T, B>::Sub(*this, rhs);
        return *this;
    }

    template <typename T, MathBackend B>
    Quaternion<T, B>& Quaternion<T, B>::operator*=(T rhs)
    {
        *this = Internal::QuatOps<T, B>::MulScalar(*this, rhs);
        return *this;
    }

    template <typename T, MathBackend B>
    Quaternion<T, B>& Quaternion<T, B>::operator*=(const Quaternion& rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    template <typename T, MathBackend B>
    Quaternion<T, B>& Quaternion<T, B>::operator/=(T rhs)
    {
        *this = Internal::QuatOps<T, B>::DivScalar(*this, rhs);
        return *this;
    }

    template <typename T, MathBackend B>
    Vec<T, 3, B> Quaternion<T, B>::ImaginaryPart() const
    {
        return Vec<T, 3, B>(this->x, this->y, this->z);
    }

    template <typename T, MathBackend B>
    T Quaternion<T, B>::Model() const
    {
        return std::sqrt(Internal::QuatOps<T, B>::Dot(*this, *this));
    }

    template <typename T, MathBackend B>
    Quaternion<T, B> Quaternion<T, B>::Negatived() const
    {
        Quaternion result;
        result.w = -this->w;
        result.x = -this->x;
        result.y = -this->y;
        result.z = -this->z;
        return result;
    }

    template <typename T, MathBackend B>
    Quaternion<T, B> Quaternion<T, B>::Conjugated() const
    {
        Quaternion result;
        result.w = this->w;
        result.x = -this->x;
        result.y = -this->y;
        result.z = -this->z;
        return result;
    }

    template <typename T, MathBackend B>
    Quaternion<T, B> Quaternion<T, B>::Normalized() const
    {
        return this->operator/(Model());
    }

    template <typename T, MathBackend B>
    T Quaternion<T, B>::Dot(const Quaternion& rhs) const
    {
        return Internal::QuatOps<T, B>::Dot(*this, rhs);
    }

    template <typename T, MathBackend B>
    Vec<T, 3, B> Quaternion<T, B>::RotateVector(const Vec<T, 3, B>& inVector) const
    {
        Quaternion v = Quaternion(0, inVector.x, inVector.y, inVector.z);
        Quaternion v2 = Conjugated() * v * (*this);
        return Vec<T, 3, B>(v2.x, v2.y, v2.z);
    }

    template <typename T, MathBackend B>
    Mat<T, 4, 4, B> Quaternion<T, B>::GetRotationMatrix() const
    {
        T xx2 = this->x * this->x * 2;
        T yy2 = this->y * this->y * 2;
        T zz2 = this->z * this->z * 2;

        T wx2 = this->w * this->x * 2;
        T wy2 = this->w * this->y * 2;
        T wz2 = this->w * this->z * 2;
        T xy2 = this->x * this->y * 2;
        T xz2 = this->x * this->z * 2;
        T yz2 = this->y * this->z * 2;

        return Mat<T, 4, 4, B>(
            1 - yy2 - zz2, xy2 + wz2, xz2 - wy2, 0,
            xy2 - wz2, 1 - xx2 - zz2, yz2 + wx2, 0,
            xz2 + wy2, yz2 - wx2, 1 - xx2 - yy2, 0,
            0, 0, 0, 1
        );
    }

    template <typename T, MathBackend B>
    template <typename IT>
    Quaternion<IT, B> Quaternion<T, B>::CastTo() const
    {
        Quaternion<IT, B> result;
        result.w = static_cast<IT>(this->w);
        result.x = static_cast<IT>(this->x);
        result.y = static_cast<IT>(this->y);
        result.z = static_cast<IT>(this->z);
        return result;
    }
}
