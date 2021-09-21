//
// Created by John Kindem on 2021/5/4.
//

#ifndef EXPLOSION_MATH_H
#define EXPLOSION_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Explosion::Math {
    template <int L>
    using Vector = glm::vec<L, float, glm::defaultp>;

    using Vector2 = Vector<2>;
    using Vector3 = Vector<3>;
    using Vector4 = Vector<4>;

    template <int L>
    using Matrix = glm::mat<L, L, float, glm::defaultp>;

    using Matrix3 = Matrix<3>;
    using Matrix4 = Matrix<4>;

    using Quaternion = glm::qua<float, glm::defaultp>;
    
    namespace ConstVector {
        const Vector<3> RIGHT = Vector<3>(1.f, 0.f, 0.f);
        const Vector<3> UP = Vector<3>(0.f, 1.f, 0.f);
        const Vector<3> FRONT = Vector<3>(0.f, 0.f, 1.f);
        const Vector<3> ZERO = Vector<3>(0.f, 0.f, 0.f);
        const Vector<3> UNIT = Vector<3>(1.f, 1.f, 1.f);
    }

    namespace ConstMatrix {
        const Matrix<4> ZERO = Matrix<4>(0.f);
        const Matrix<4> UNIT = Matrix<4>(1.f);
    }

    namespace ConstQuaternion {
        const Quaternion UNIT = glm::identity<Quaternion>();
    }

    namespace Calculator {
        template <int L>
        inline Matrix<L> Transpose(const Matrix<L>& matrix);

        inline float Radian(float degree);

        inline Matrix<4> Translate(const Matrix<4>& matrix, const Vector<3>& value);
        inline Matrix<4> Rotate(const Matrix<4>& matrix, const Vector<3>& axis, float radian);
        inline Matrix<4> Scale(const Matrix<4>& matrix, const Vector<3>& value);
    }
}

#endif //EXPLOSION_MATH_H
