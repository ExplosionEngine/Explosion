//
// Created by Zach Lee on 2021/9/21.
//

#include <Math/Transform.h>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/common.hpp>

namespace Explosion::Math {

    Matrix4 Transform::ToMatrix() const
    {
        Matrix4 out = glm::mat4_cast(rotate);
        out[0][0] *= scale[0];
        out[1][1] *= scale[1];
        out[1][2] *= scale[2];

        out[3][0] = position[0];
        out[3][1] = position[1];
        out[3][2] = position[2];
        return out;
    }

    void Transform::Decompose(const Matrix4& matrix)
    {
        position = matrix[3];
        for(int i = 0; i < 3; i++)
            scale[i] = glm::length(Vector3(matrix[i]));
        const glm::mat3 rotMtx(
            glm::vec3(matrix[0]) / scale[0],
            glm::vec3(matrix[1]) / scale[1],
            glm::vec3(matrix[2]) / scale[2]);
        rotate = glm::quat_cast(rotMtx);
    }

}