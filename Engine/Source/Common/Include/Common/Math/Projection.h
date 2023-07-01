//
// Created by johnk on 2023/6/26.
//

#pragma once

#include <optional>

#include <Common/Math/Matrix.h>

namespace Common {
    template <typename T>
    struct ReversedZOrthogonalProjection {
        ReversedZOrthogonalProjection(T inWidth, T inHeight, T inNearPlane);
        ReversedZOrthogonalProjection(T inWidth, T inHeight, T inNearPlane, T inFarPlane);
        Matrix<T, 4, 4> GetProjectionMatrix() const;

        T width;
        T height;
        T nearPlane;
        std::optional<T> farPlane;
    };

    template <typename T>
    struct ReversedZPerspectiveProjection {
        ReversedZPerspectiveProjection(T inFOV, T inWidth, T inHeight, T inNearPlane);
        ReversedZPerspectiveProjection(T inFOV, T inWidth, T inHeight, T inNearPlane, T inFarPlane);
        Matrix<T, 4, 4> GetProjectionMatrix() const;

        T fov;
        T width;
        T height;
        T nearPlane;
        std::optional<T> farPlane;
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
        : width(inWidth), height(inHeight), nearPlane(inNearPlane), farPlane()
    {
    }

    template <typename T>
    ReversedZOrthogonalProjection<T>::ReversedZOrthogonalProjection(T inWidth, T inHeight, T inNearPlane, T inFarPlane)
        : width(inWidth), height(inHeight), nearPlane(inNearPlane), farPlane(inFarPlane)
    {
    }

    template <typename T>
    Matrix<T, 4, 4> ReversedZOrthogonalProjection<T>::GetProjectionMatrix() const
    {
        // TOOD
        return Matrix<T, 4, 4>();
    }

    template <typename T>
    ReversedZPerspectiveProjection<T>::ReversedZPerspectiveProjection(T inFOV, T inWidth, T inHeight, T inNearPlane)
        : fov(inFOV), width(inWidth), height(inHeight), nearPlane(inNearPlane), farPlane()
    {
    }

    template <typename T>
    ReversedZPerspectiveProjection<T>::ReversedZPerspectiveProjection(T inFOV, T inWidth, T inHeight, T inNearPlane, T inFarPlane)
        : fov(inFOV), width(inWidth), height(inHeight), nearPlane(inNearPlane), farPlane(inFarPlane)
    {
    }

    template <typename T>
    Matrix<T, 4, 4> ReversedZPerspectiveProjection<T>::GetProjectionMatrix() const
    {
        // TODO
        return Matrix<T, 4, 4>();
    }
}
