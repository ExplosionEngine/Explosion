//
// Created by johnk on 2023/6/26.
//

#pragma once

#include <optional>

#include <Common/Math/Matrix.h>
#include <Common/Math/Quaternion.h>

namespace Common {
    template <typename T>
    requires isFloatingPointV<T>
    struct ReversedZOrthogonalProjectionBase {
        T width;
        T height;
        T nearPlane;
        std::optional<T> farPlane;
    };

    template <typename T>
    requires isFloatingPointV<T>
    struct ReversedZPerspectiveProjectionBase {
        T fov;
        T width;
        T height;
        T nearPlane;
        std::optional<T> farPlane;
    };

    template <typename T>
    struct ReversedZOrthogonalProjection : public ReversedZOrthogonalProjectionBase<T> {
        inline ReversedZOrthogonalProjection(T inWidth, T inHeight, T inNearPlane);
        inline ReversedZOrthogonalProjection(T inWidth, T inHeight, T inNearPlane, T inFarPlane);
        inline Matrix<T, 4, 4> GetProjectionMatrix() const;
    };

    template <typename T>
    struct ReversedZPerspectiveProjection : public ReversedZPerspectiveProjectionBase<T> {
        inline ReversedZPerspectiveProjection(T inFOV, T inWidth, T inHeight, T inNearPlane);
        inline ReversedZPerspectiveProjection(T inFOV, T inWidth, T inHeight, T inNearPlane, T inFarPlane);
        inline Matrix<T, 4, 4> GetProjectionMatrix() const;
    };

    using HReversedZOrthoProjection = ReversedZOrthogonalProjection<HFloat>;
    using FReversedZOrthoProjection = ReversedZOrthogonalProjection<float>;
    using DReversedZOrthoProjection = ReversedZOrthogonalProjection<double>;

    using HReversedZPerspectiveProjection = ReversedZPerspectiveProjection<HFloat>;
    using FReversedZPerspectiveProjection = ReversedZPerspectiveProjection<float>;
    using DReversedZPerspectiveProjection = ReversedZPerspectiveProjection<double>;
}

namespace Common {
    template <typename T>
    ReversedZOrthogonalProjection<T>::ReversedZOrthogonalProjection(T inWidth, T inHeight, T inNearPlane)
    {
        this->width = inWidth;
        this->height = inHeight;
        this->nearPlane = inNearPlane;
        this->farPlane = {};
    }

    template <typename T>
    ReversedZOrthogonalProjection<T>::ReversedZOrthogonalProjection(T inWidth, T inHeight, T inNearPlane, T inFarPlane)
    {
        this->width = inWidth;
        this->height = inHeight;
        this->nearPlane = inNearPlane;
        this->farPlane = inFarPlane;
    }

    template <typename T>
    Matrix<T, 4, 4> ReversedZOrthogonalProjection<T>::GetProjectionMatrix() const
    {
        if (this->farPlane.has_value()) {
            return Matrix<T, 4, 4>(
                2.0f / this->width, 0.0f, 0.0f, 0.0f,
                0.0f, 2.0f / this->height, 0.0f, 0.0f,
                0.0f, 0.0f, -1.0f / (this->farPlane.value() - this->nearPlane), 1.0f + (this->nearPlane / (this->farPlane.value() - this->nearPlane)),
                0.0f, 0.0f, 0.0f, 1.0f
            );
        } else {
            // Infinite Far Plane
            return Matrix<T, 4, 4>(
                2.0f / this->width, 0.0f, 0.0f, 0.0f,
                0.0f, 2.0f / this->height, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            );
        }
    }

    template <typename T>
    ReversedZPerspectiveProjection<T>::ReversedZPerspectiveProjection(T inFOV, T inWidth, T inHeight, T inNearPlane)
    {
        this->fov = inFOV;
        this->width = inWidth;
        this->height = inHeight;
        this->nearPlane = inNearPlane;
        this->farPlane = {};
    }

    template <typename T>
    ReversedZPerspectiveProjection<T>::ReversedZPerspectiveProjection(T inFOV, T inWidth, T inHeight, T inNearPlane, T inFarPlane)
    {
        this->fov = inFOV;
        this->width = inWidth;
        this->height = inHeight;
        this->nearPlane = inNearPlane;
        this->farPlane = inFarPlane;
    }

    template <typename T>
    Matrix<T, 4, 4> ReversedZPerspectiveProjection<T>::GetProjectionMatrix() const
    {
        Angle<T> angle(this->fov);
        T tanHalfFov = tan(angle.ToRadian() / static_cast<T>(2));

        if (this->farPlane.has_value()) {
            return Matrix<T, 4, 4>(
                this->height / (this->width * tanHalfFov), 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
                0.0f, 0.0f, this->nearPlane / (this->nearPlane - this->farPlane.value()), this->nearPlane * this->farPlane.value() / (this->farPlane.value() - this->nearPlane),
                0.0f, 0.0f, 1.0f, 0.0f
            );
        } else {
            // Infinite Far Plane
            return Matrix<T, 4, 4>(
                this->height / (this->width * tanHalfFov), 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, this->nearPlane,
                0.0f, 0.0f, 1.0f, 0.0f
            );
        }
    }
}
