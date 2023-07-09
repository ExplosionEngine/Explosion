//
// Created by johnk on 2023/6/26.
//

#pragma once

#include <optional>

#include <Common/Math/Matrix.h>

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
        // TOOD
        return Matrix<T, 4, 4>();
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
        // TODO
        return Matrix<T, 4, 4>();
    }
}
