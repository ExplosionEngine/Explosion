

namespace Common {

    inline Quaternion::Quaternion() : Quaternion(1, 0, 0, 0)
    {
    }

    inline Quaternion::Quaternion(float w_, float x_, float y_, float z_) : x(x_), y(y_), z(z_), w(w_)
    {
    }

    inline Quaternion::Quaternion(float angle, const Vector3 &axis)
    {
        float const half = angle * 0.5f;
        float const s = sin(half);
        float const c = cos(half);
        Vector3 tmp = axis * s;
        x = tmp.x;
        y = tmp.y;
        z = tmp.z;
        w = c;
    }

    inline void Quaternion::Normalize()
    {
        float n = w * w + x * x + y * y + z * z;
        if (n == 0)
        {
            w = 1;
            x = 0;
            y = 0;
            z = 0;
        }
        float inverseSqrt = 1 / sqrt(n);
        Quaternion::operator*=(inverseSqrt);
    }

    inline Quaternion Quaternion::Conjugate() const
    {
        return {w, -x, -y, -z};
    }

    inline Quaternion Quaternion::operator*(const Quaternion &rhs) const
    {
        Quaternion res;
        res.w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
        res.x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
        res.y = w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z;
        res.z = w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x;
        return res;
    }

    inline Vector3 Quaternion::operator*(const Vector3 &rhs) const
    {
        Vector3 const tmpVec(x, y, z);
        Vector3 const uv(tmpVec.Cross(rhs));
        Vector3 const uuv(tmpVec.Cross(uv));

        return rhs + ((uv * w) + uuv) * 2.f;
    }

    inline Quaternion &Quaternion::operator*=(float m)
    {
        x *= m;
        y *= m;
        z *= m;
        return *this;
    }

    inline Quaternion &Quaternion::operator/=(float d)
    {
        x /= d;
        y /= d;
        z /= d;
        return *this;
    }
} // namespace Common