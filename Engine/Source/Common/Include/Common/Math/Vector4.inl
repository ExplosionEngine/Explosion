//
// Created by Zach Lee on 2022/9/10.
//

namespace Common {

    inline Vector4::Vector4() : Vector4(0, 0, 0, 0)
    {
    }

    inline Vector4::Vector4(float v) : Vector4(v, v, v, v)
    {
    }

    inline Vector4::Vector4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_)
    {
    }

    inline Vector4 Vector4::operator+(const Vector4& rhs) const
    {
        return Vector4(*this) += rhs;
    }

    inline Vector4 Vector4::operator-() const
    {
        return Vector4(0) - *this;
    }

    inline Vector4 Vector4::operator-(const Vector4& rhs) const
    {
        return Vector4(*this) -= rhs;
    }

    inline Vector4 Vector4::operator*(const Vector4& rhs) const
    {
        return Vector4(*this) *= rhs;
    }

    inline Vector4 Vector4::operator*(float m) const
    {
        return Vector4(*this) *= m;
    }

    inline Vector4 Vector4::operator/(const Vector4& rhs) const
    {
        return Vector4(*this) /= rhs;
    }

    inline Vector4 Vector4::operator/(float d) const
    {
        return Vector4(*this) /= d;
    }

    inline Vector4& Vector4::operator+=(const Vector4& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    inline Vector4& Vector4::operator-=(const Vector4& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    inline Vector4& Vector4::operator*=(const Vector4& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        w *= rhs.w;
        return *this;
    }

    inline Vector4& Vector4::operator/=(const Vector4& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        w /= rhs.w;
        return *this;
    }

    inline Vector4& Vector4::operator*=(float m)
    {
        x *= m;
        y *= m;
        z *= m;
        w *= m;
        return *this;
    }

    inline Vector4& Vector4::operator/=(float d)
    {
        x /= d;
        y /= d;
        z /= d;
        w /= d;
        return *this;
    }

    inline float &Vector4::operator[](uint32_t i)
    {
        return v[i];
    }

    inline float Vector4::operator[](uint32_t i) const
    {
        return v[i];
    }

    inline float Vector4::Dot(const Vector4 &rhs) const
    {
        Vector4 ret = (*this) * rhs;
        return (ret.x + ret.y) + (ret.z + ret.w);
    }

    inline void Vector4::Normalize()
    {
        float inverseSqrt = 1 / sqrt(Dot(*this));
        Vector4::operator*=(inverseSqrt);
    }
} // namespace Common
