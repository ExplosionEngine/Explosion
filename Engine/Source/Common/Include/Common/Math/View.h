//
// Created by johnk on 2023/7/7.
//

#pragma once

#include <Common/Math/Transform.h>

namespace Common {
    template <typename T>
    struct ViewTransform : public Transform<T> {
        inline static ViewTransform LookAt(const Vector<T, 3>& inPosition, const Vector<T, 3>& inTargetPosition, const Vector<T, 3>& inUpDirection = VecConsts<T, 3>::unitZ);

        inline ViewTransform();
        inline ViewTransform(Quaternion<T> inRotation, Vector<T, 3> inTranslation);
        inline explicit ViewTransform(const Transform<T>& inTransform);
        inline ViewTransform(const ViewTransform& inOther);
        inline ViewTransform(ViewTransform&& inOther) noexcept;
        inline ViewTransform& operator=(const ViewTransform& inOther);

        inline Matrix<T, 4, 4> GetViewMatrix();
    };

    using HViewTransform = ViewTransform<HFloat>;
    using FViewTransform = ViewTransform<float>;
    using DViewTransform = ViewTransform<double>;
}

namespace Common {
    template <typename T>
    ViewTransform<T> ViewTransform<T>::LookAt(const Vector<T, 3>& inPosition, const Vector<T, 3>& inTargetPosition, const Vector<T, 3>& inUpDirection)
    {
        return Transform<T>::LookAt(inPosition, inTargetPosition, inUpDirection);
    }

    template <typename T>
    ViewTransform<T>::ViewTransform()
        : Transform<T>()
    {
    }

    template <typename T>
    ViewTransform<T>::ViewTransform(Quaternion<T> inRotation, Vector<T, 3> inTranslation)
        : Transform<T>(std::move(inRotation), std::move(inTranslation))
    {
    }

    template <typename T>
    ViewTransform<T>::ViewTransform(const Transform<T>& inTransform)
        : Transform<T>(inTransform)
    {
    }

    template <typename T>
    ViewTransform<T>::ViewTransform(const ViewTransform& inOther)
        : Transform<T>(inOther)
    {
    }

    template <typename T>
    ViewTransform<T>::ViewTransform(ViewTransform&& inOther) noexcept
        : Transform<T>(std::move(inOther))
    {
    }

    template <typename T>
    ViewTransform<T>& ViewTransform<T>::operator=(const ViewTransform& inOther)
    {
        Transform<T>::operator=(inOther);
        return *this;
    }

    template <typename T>
    Matrix<T, 4, 4> ViewTransform<T>::GetViewMatrix()
    {
        // before apply axis transform matrix:
        //     x+ -> from screen outer to inner
        //     y+ -> from left to right
        //     z+ -> from bottom to top
        // after apply axis transform matrix:
        //     x+ -> from left to right
        //     y+ -> from bottom to top
        //     z+ -> from screen outer to inner
        static Matrix<T, 4, 4> axisTransMat = Matrix<T, 4, 4>(
            0, 1, 0, 0,
            0, 0, 1, 0,
            1, 0, 0, 0,
            0, 0, 0, 1
        );

        // To transform an object, scale first, then rotate and translate last,
        // we need apply the inverse transform(rotation and translation) of camera to visible objects,
        // but the order(rotate then translate) can not be changed,
        // (m1 * m2).Inverse() equals to m2.Inverse() * m1.Inverse()
        return this->GetTransformMatrixNoScale().Inverse() * axisTransMat;
    }
}