//
// Created by Zach Lee on 2022/9/10.
//

namespace Common {

    inline Vector3::Vector3() : Vector3(0, 0, 0)
    {
    }

    inline Vector3::Vector3(float v) : Vector3(v, v, v)
    {
    }

    inline Vector3::Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_)
    {
    }

    inline Vector3 Vector3::operator+(const Vector3& rhs) const
    {
        return Vector3(*this) += rhs;
    }

    inline Vector3 Vector3::operator-() const
    {
        return Vector3(0) - *this;
    }

    inline Vector3 Vector3::operator-(const Vector3& rhs) const
    {
        return Vector3(*this) -= rhs;
    }

    inline Vector3 Vector3::operator*(const Vector3& rhs) const
    {
        return Vector3(*this) *= rhs;
    }

    inline Vector3 Vector3::operator*(float m) const
    {
        return Vector3(*this) *= m;
    }

    inline Vector3 Vector3::operator/(const Vector3& rhs) const
    {
        return Vector3(*this) /= rhs;
    }

    inline Vector3 Vector3::operator/(float d) const
    {
        return Vector3(*this) /= d;
    }

    inline Vector3& Vector3::operator+=(const Vector3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    inline Vector3& Vector3::operator-=(const Vector3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    inline Vector3& Vector3::operator*=(const Vector3& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    inline Vector3& Vector3::operator/=(const Vector3& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    inline Vector3& Vector3::operator*=(float m)
    {
        x *= m;
        y *= m;
        z *= m;
        return *this;
    }

    inline Vector3& Vector3::operator/=(float d)
    {
        x /= d;
        y /= d;
        z /= d;
        return *this;
    }

    inline float &Vector3::operator[](uint32_t i)
    {
        return v[i];
    }

    inline float Vector3::operator[](uint32_t i) const
    {
        return v[i];
    }

    inline Vector3 Vector3::Cross(const Vector3 &rhs) const
    {
        return {y * rhs.z - rhs.y * z, z * rhs.x - rhs.z * x, x * rhs.y - rhs.x * y};
    }

    inline float Vector3::Dot(const Vector3 &rhs) const
    {
        Vector3 ret = (*this) * rhs;
        return ret.x + ret.y + ret.z;
    }

    inline void Vector3::Normalize()
    {
        float inverseSqrt = 1 / sqrt(Dot(*this));
        Vector3::operator*=(inverseSqrt);
    }
} // namespace Common