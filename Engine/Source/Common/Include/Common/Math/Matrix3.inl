

namespace Common {

    inline Matrix3::Matrix3() : Matrix3(Vector3(), Vector3(), Vector3())
    {

    }

    inline Matrix3::Matrix3(const Vector3 &r0, const Vector3 &r1, const Vector3 &r2)
        : m{r0, r1, r2}
    {
    }

    inline Matrix3 Matrix3::FromEulerYXZ(const Euler &euler)
    {
        float ch  = cos(euler.y);
        float sh  = sin(euler.y);
        float cp  = cos(euler.x);
        float sp  = sin(euler.x);
        float cb  = cos(euler.z);
        float sb  = sin(euler.z);

        Matrix3 ret;
        ret[0][0] = ch * cb + sh * sp * sb;
        ret[0][1] = sb * cp;
        ret[0][2] = -sh * cb + ch * sp * sb;
        ret[1][0] = -ch * sb + sh * sp * cb;
        ret[1][1] = cb * cp;
        ret[1][2] = sb * sh + ch * sp * cb;
        ret[2][0] = sh * cp;
        ret[2][1] = -sp;
        ret[2][2] = ch * cp;
        return ret;
    }

    inline const Matrix3 &Matrix3::Identity()
    {
        static Matrix3 matrix;
        matrix.m[0][0] = 1.f;
        matrix.m[1][1] = 1.f;
        matrix.m[2][2] = 1.f;
        return matrix;
    }

    inline Matrix3 Matrix3::Inverse() const
    {
        float inverseDet = 1.f / (m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
                                  m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2]) +
                                  m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));

        Matrix3 ret;
        ret[0][0] = +(m[1][1] * m[2][2] - m[2][1] * m[1][2]) * inverseDet;
        ret[1][0] = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]) * inverseDet;
        ret[2][0] = +(m[1][0] * m[2][1] - m[2][0] * m[1][1]) * inverseDet;
        ret[0][1] = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]) * inverseDet;
        ret[1][1] = +(m[0][0] * m[2][2] - m[2][0] * m[0][2]) * inverseDet;
        ret[2][1] = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]) * inverseDet;
        ret[0][2] = +(m[0][1] * m[1][2] - m[1][1] * m[0][2]) * inverseDet;
        ret[1][2] = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]) * inverseDet;
        ret[2][2] = +(m[0][0] * m[1][1] - m[1][0] * m[0][1]) * inverseDet;

        return ret;
    }

    inline Matrix3 Matrix3::operator+(const Matrix3& rhs) const
    {
        return Matrix3(*this) += rhs;
    }

    inline Matrix3 Matrix3::operator-(const Matrix3& rhs) const
    {
        return Matrix3(*this) -= rhs;
    }

    inline Matrix3 Matrix3::operator*(const Matrix3& rhs) const
    {
        return Matrix3(*this) *= rhs;
    }

    inline Matrix3 Matrix3::operator*(float multiplier) const
    {
        return Matrix3(*this) *= multiplier;
    }

    inline Matrix3 Matrix3::operator/(float divisor) const
    {
        return Matrix3(*this) /= divisor;
    }

    inline Matrix3 Matrix3::operator-() const
    {
        return Matrix3() - (*this);
    }

    inline Matrix3& Matrix3::operator+=(const Matrix3& rhs)
    {
        for (uint32_t i = 0; i < 4; ++i) {
            m[i] += rhs.m[i];
        }
        return *this;
    }

    inline Matrix3& Matrix3::operator-=(const Matrix3& rhs)
    {
        for (uint32_t i = 0; i < 4; ++i) {
            m[i] -= rhs.m[i];
        }
        return *this;
    }

    inline Matrix3& Matrix3::operator*=(const Matrix3& rhs)
    {
        for (uint32_t i = 0; i < 4; ++i) {
            m[i] *= rhs.m[i];
        }
        return *this;
    }

    inline Matrix3& Matrix3::operator*=(float multiplier)
    {
        for (uint32_t i = 0; i < 4; ++i) {
            m[i] *= multiplier;
        }
        return *this;
    }

    inline Matrix3& Matrix3::operator/=(float divisor)
    {
        for (uint32_t i = 0; i < 4; ++i) {
            m[i] /= divisor;
        }
        return *this;
    }

    inline Vector3 Matrix3::operator*(const Vector3& rhs) const
    {
        Vector3 v0 = m[0] * rhs[0];
        Vector3 v1 = m[1] * rhs[1];
        Vector3 v2 = m[2] * rhs[2];
        return (v0 + v1) + v2;
    }

    inline Vector3 &Matrix3::operator[](uint32_t i)
    {
        return m[i];
    }

    inline Vector3 Matrix3::operator[](uint32_t i) const
    {
        return m[i];
    }


} // namespace Common