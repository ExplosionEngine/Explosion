//
// Created by Junkang on 2023/10/06.
//

#pragma once

#include <array>
#include <unordered_map>

#include <Common/Math/Matrix.h>
#include <Common/Math/Transform.h>
#include <Common/Math/Projection.h>
#include <Common/Math/View.h>

using namespace Common;

class Camera {
public:
    enum class MoveDirection {
        front,
        back,
        left,
        right,
        up,
        down,
        max
    };

    struct ProjectionParams {
        float fov;
        float width;
        float height;
        float nearPlane;
        float farPlane;
    };

    // view space means the local space of camera
    // all the calculation applies to camera is in world space
    // transform is the operations make camera from local to world
    // the inverse of transform is the operations make objects in world from world to view(the local of camera)
    Camera(const FVec3& inPos, const FVec3& inRotEulerZYX, const ProjectionParams& inProjParams);

    void SetPosition(const FVec3& inPosition);
    void SetRotation(const FVec3& inRotEulerZYX);
    void SetLookTarget(const FVec3& inTarget);
    void SetTranslation(const FVec3& inTranslation);
    void Translate(const FVec3& inTransDelta);
    void Rotate(const FVec3& inRotDelta);
    void PerformMove(MoveDirection direction);
    void PerformStop(MoveDirection direction);
    void SetMoveSpeed(float inSpeed);
    void SetRotateSpeed(float inSpeed);
    void Update(float frameTime);
    FMat4x4 GetProjectionMatrix() const;
    FMat4x4 GetViewMatrix() const;
    float GetMoveSpeed() const;
    float GetRotateSpeed() const;

private:
    bool Moving() const;

    // vt makes camera from local to world
    FViewTransform viewTransform;
    FReversedZPerspectiveProjection projection;
    FVec3 lookTarget;
    std::array<bool, static_cast<size_t>(MoveDirection::max)> movingStatus;
    std::unordered_map<MoveDirection, FVec3> moveVectorMap;
    float moveSpeed;
    float rotateSpeed;
};