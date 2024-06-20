//
// Created by johnk on 2024/5/20.
//

#include <Camera.h>

Camera::Camera(const FVec3& inPos, const FVec3& inRotEulerZYX, const ProjectionParams& inProjParams)
    : viewTransform(FQuat::FromEulerZYX(inRotEulerZYX.x, inRotEulerZYX.y, inRotEulerZYX.z), inPos)
    , projection(inProjParams.fov, inProjParams.width, inProjParams.height, inProjParams.nearPlane, inProjParams.farPlane)
    , lookTarget(FVec3Consts::unitZ)
    , movingStatus()
    , moveSpeed(1.0f)
    , rotateSpeed(1.0f)
{
    moveVectorMap = {
        { MoveDirection::front, FVec3Consts::unitX },
        { MoveDirection::back, FVec3Consts::negaUnitX },
        { MoveDirection::left, FVec3Consts::negaUnitY },
        { MoveDirection::right, FVec3Consts::unitY },
        { MoveDirection::up, FVec3Consts::unitZ },
        { MoveDirection::down, FVec3Consts::negaUnitZ }
    };
}

void Camera::SetPosition(const FVec3& inPosition)
{
    viewTransform.translation = inPosition;
}

void Camera::SetRotation(const FVec3& inRotEulerZYX)
{
    viewTransform.rotation = FQuat::FromEulerZYX(inRotEulerZYX.x, inRotEulerZYX.y, inRotEulerZYX.z);
}

void Camera::SetLookTarget(const FVec3& inTarget)
{
    lookTarget = inTarget;
    viewTransform.LookTo(lookTarget);
}

void Camera::SetTranslation(const FVec3& inTranslation)
{
    viewTransform.translation = inTranslation;
}

void Camera::Translate(const FVec3& inTransDelta)
{
    // inTransDelta is in camera's local, needs to be changed to world
    viewTransform.translation += viewTransform.rotation.RotateVector(inTransDelta);
}

void Camera::Rotate(const FVec3& inRotDelta)
{
    // inRotDelta is also in camera's local, but we need't to change it to world, because previous rotation will be applied first, making the coords in camera local
    // imagine putting a new camera from wolrd's origin to the position of old camera using old transform
    viewTransform.rotation = FQuat::FromEulerZYX(inRotDelta.x, inRotDelta.y, inRotDelta.z) * viewTransform.rotation;
}

void Camera::PerformMove(MoveDirection direction)
{
    movingStatus[static_cast<size_t>(direction)] = true;
}

void Camera::PerformStop(MoveDirection direction)
{
    movingStatus[static_cast<size_t>(direction)] = false;
}

void Camera::SetMoveSpeed(float inSpeed)
{
    moveSpeed = inSpeed;
}

void Camera::SetRotateSpeed(float inSpeed)
{
    rotateSpeed = inSpeed;
}

void Camera::Update(float frameTime)
{
    if (!Moving()) {
        return;
    }

    const float frameMovement = moveSpeed * frameTime;
    FVec3 posDelta = FVec3Consts::zero;
    for (auto i = 0; i < movingStatus.size(); i++) {
        if (movingStatus[i]) {
            posDelta += moveVectorMap.at(static_cast<MoveDirection>(i)) * frameMovement;
        }
    }
    Translate(posDelta);
}

FMat4x4 Camera::GetProjectionMatrix() const
{
    return projection.GetProjectionMatrix();
}

FMat4x4 Camera::GetViewMatrix() const
{
    return viewTransform.GetViewMatrix();
}

float Camera::GetMoveSpeed() const
{
    return moveSpeed;
}

float Camera::GetRotateSpeed() const
{
    return rotateSpeed;
}

bool Camera::Moving() const
{
    bool result = false;
    for (auto i = 0; i < movingStatus.size(); i++) {
        result = result || movingStatus[i];
    }
    return result;
}
