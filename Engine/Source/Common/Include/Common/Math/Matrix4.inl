
namespace Common {

    inline Matrix4::Matrix4() : Matrix4(Vector4(), Vector4(), Vector4(), Vector4())
    {
    }

    inline Matrix4::Matrix4(const Vector4 &r0, const Vector4 &r1, const Vector4 &r2, const Vector4 &r3)
        : m{r0, r1, r2, r3}
    {
    }

    inline const Matrix4 &Matrix4::Identity()
    {
        static Matrix4 matrix;
        matrix.m[0][0] = 1.f;
        matrix.m[1][1] = 1.f;
        matrix.m[2][2] = 1.f;
        matrix.m[3][3] = 1.f;
        return matrix;
    }

    inline Matrix4 Matrix4::operator+(const Matrix4& rhs) const
    {
        return Matrix4(*this) += rhs;
    }

    inline Matrix4 Matrix4::operator-(const Matrix4& rhs) const
    {
        return Matrix4(*this) -= rhs;
    }

    inline Matrix4 Matrix4::operator*(const Matrix4& rhs) const
    {
        const auto& rw = rhs.m;
        Matrix4 ret;
        ret[0] = m[0] * rw[0][0] + m[1] * rw[0][1] + m[2] * rw[0][2] + m[3] * rw[0][3];
        ret[1] = m[0] * rw[1][0] + m[1] * rw[1][1] + m[2] * rw[1][2] + m[3] * rw[1][3];
        ret[2] = m[0] * rw[2][0] + m[1] * rw[2][1] + m[2] * rw[2][2] + m[3] * rw[2][3];
        ret[3] = m[0] * rw[3][0] + m[1] * rw[3][1] + m[2] * rw[3][2] + m[3] * rw[3][3];
        return ret;
    }

    inline Matrix4 Matrix4::operator*(float multiplier) const
    {
        return Matrix4(*this) *= multiplier;
    }

    inline Matrix4 Matrix4::operator/(float divisor) const
    {
        return Matrix4(*this) /= divisor;
    }

    inline Matrix4 Matrix4::operator-() const
    {
        return Matrix4() - (*this);
    }

    inline Matrix4& Matrix4::operator+=(const Matrix4& rhs)
    {
        for (uint32_t i = 0; i < 4; ++i) {
            m[i] += rhs.m[i];
        }
        return *this;
    }

    inline Matrix4& Matrix4::operator-=(const Matrix4& rhs)
    {
        for (uint32_t i = 0; i < 4; ++i) {
            m[i] -= rhs.m[i];
        }
        return *this;
    }

    inline Matrix4& Matrix4::operator*=(const Matrix4& rhs)
    {
        return *this = (*this) * rhs;
    }

    inline Matrix4& Matrix4::operator*=(float multiplier)
    {
        for (auto & i : m) {
            i *= multiplier;
        }
        return *this;
    }

    inline Matrix4& Matrix4::operator/=(float divisor)
    {
        for (auto & i : m) {
            i /= divisor;
        }
        return *this;
    }

    inline Vector4 Matrix4::operator*(const Vector4& rhs) const
    {
        Vector4 v0 = m[0] * rhs[0];
        Vector4 v1 = m[1] * rhs[1];
        Vector4 v2 = m[2] * rhs[2];
        Vector4 v3 = m[3] * rhs[3];
        return (v0 + v1) + (v2 + v3);
    }

    inline Vector4 &Matrix4::operator[](uint32_t i)
    {
        return m[i];
    }

    inline Vector4 Matrix4::operator[](uint32_t i) const
    {
        return m[i];
    }

    inline void Matrix4::Translate(const Vector3 &rhs)
    {
        m[3] = m[0] * rhs.v[0] + m[1] * rhs.v[1] + m[2] * rhs.v[2] + m[3];
    }

    inline Matrix4 Matrix4::Inverse() const
    {
        float c00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
        float c02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
        float c03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

        float c04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
        float c06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
        float c07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

        float c08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
        float c10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
        float c11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

        float c12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
        float c14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
        float c15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

        float c16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
        float c18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
        float c19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

        float c20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
        float c22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
        float c23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

        Vector4 fac0(c00, c00, c02, c03);
        Vector4 fac1(c04, c04, c06, c07);
        Vector4 fac2(c08, c08, c10, c11);
        Vector4 fac3(c12, c12, c14, c15);
        Vector4 fac4(c16, c16, c18, c19);
        Vector4 fac5(c20, c20, c22, c23);

        Vector4 vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
        Vector4 vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
        Vector4 vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
        Vector4 vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

        Vector4 inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
        Vector4 inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
        Vector4 inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
        Vector4 inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

        Vector4 signA(+1, -1, +1, -1);
        Vector4 signB(-1, +1, -1, +1);
        Matrix4 inverse(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);

        Vector4 row0(inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0]);

        Vector4 dot0(m[0] * row0);
        float dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

        float inverseDet = 1.f / dot1;
        return inverse * inverseDet;
    }

    inline Matrix4 Matrix4::InverseTranspose() const
    {
        float s00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
        float s01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
        float s02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
        float s03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
        float s04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
        float s05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
        float s06 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
        float s07 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
        float s08 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
        float s09 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
        float s10 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
        float s11 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
        float s12 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
        float s13 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
        float s14 = m[1][1] * m[2][3] - m[2][1] * m[1][3];
        float s15 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
        float s16 = m[1][0] * m[2][3] - m[2][0] * m[1][3];
        float s17 = m[1][0] * m[2][2] - m[2][0] * m[1][2];
        float s18 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

        Matrix4 ret;
        ret[0][0] = + (m[1][1] * s00 - m[1][2] * s01 + m[1][3] * s02);
        ret[0][1] = - (m[1][0] * s00 - m[1][2] * s03 + m[1][3] * s04);
        ret[0][2] = + (m[1][0] * s01 - m[1][1] * s03 + m[1][3] * s05);
        ret[0][3] = - (m[1][0] * s02 - m[1][1] * s04 + m[1][2] * s05);

        ret[1][0] = - (m[0][1] * s00 - m[0][2] * s01 + m[0][3] * s02);
        ret[1][1] = + (m[0][0] * s00 - m[0][2] * s03 + m[0][3] * s04);
        ret[1][2] = - (m[0][0] * s01 - m[0][1] * s03 + m[0][3] * s05);
        ret[1][3] = + (m[0][0] * s02 - m[0][1] * s04 + m[0][2] * s05);

        ret[2][0] = + (m[0][1] * s06 - m[0][2] * s07 + m[0][3] * s08);
        ret[2][1] = - (m[0][0] * s06 - m[0][2] * s09 + m[0][3] * s10);
        ret[2][2] = + (m[0][0] * s11 - m[0][1] * s09 + m[0][3] * s12);
        ret[2][3] = - (m[0][0] * s08 - m[0][1] * s10 + m[0][2] * s12);

        ret[3][0] = - (m[0][1] * s13 - m[0][2] * s14 + m[0][3] * s15);
        ret[3][1] = + (m[0][0] * s13 - m[0][2] * s16 + m[0][3] * s17);
        ret[3][2] = - (m[0][0] * s14 - m[0][1] * s16 + m[0][3] * s18);
        ret[3][3] = + (m[0][0] * s15 - m[0][1] * s17 + m[0][2] * s18);

        float det =
            + m[0][0] * ret[0][0]
            + m[0][1] * ret[0][1]
            + m[0][2] * ret[0][2]
            + m[0][3] * ret[0][3];

        ret /= det;

        return ret;
    }
} // namespace Common