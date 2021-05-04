//
// Created by John Kindem on 2021/5/4.
//

#include <Explosion/Common/Math.h>

namespace Explosion::Math::Calculator {
    template<int L>
    Matrix<L> Transpose(const Matrix<L>& matrix)
    {
        return glm::transpose(matrix);
    }

    float Radian(float degree)
    {
        return glm::radians(degree);
    }

    Matrix<4> Translate(const Matrix<4>& matrix, const Vector<3>& value)
    {
        return glm::translate(matrix, value);
    }

    Matrix<4> Rotate(const Matrix<4>& matrix, const Vector<3>& axis, float radian)
    {
        return glm::rotate(matrix, radian, axis);
    }

    Matrix<4> Scale(const Matrix<4>& matrix, const Vector<3>& value)
    {
        return glm::scale(matrix, value);
    }
}
