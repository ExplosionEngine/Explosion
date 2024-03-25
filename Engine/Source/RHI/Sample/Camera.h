//
// Created by Junkang on 2023/10/06.
//

#pragma once

#include <Common/Math/Matrix.h>
#include <Common/Math/Transform.h>
#include <Common/Math/Projection.h>
#include <Common/Math/View.h>

using namespace Common;

class Camera {
public:
    float rotateSpeed = 1.0f;
    float moveSpeed = 1.0f;

    struct {
        bool left = false;
        bool right = false;
        bool up = false;
        bool down = false;
        bool front = false;
        bool back = false;
    } keys;

    struct ProjectParams {
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
    Camera(FVec3 inPos, FVec3 inRot, ProjectParams pParam)
        : viewTransform(FQuat::FromEulerZYX(inRot.x, inRot.y, inRot.z), inPos),
        rzProjection(pParam.fov, pParam.width, pParam.height, pParam.nearPlane, pParam.farPlane)
    {
    }

    void SetPosition(FVec3 inPosition)
    {
        viewTransform.translation = inPosition;
    }

    void SetRotation(FVec3 inRotation)
    {
        viewTransform.rotation = FQuat::FromEulerZYX(inRotation.x, inRotation.y, inRotation.z);
    }

    void SetTarget(FVec3 inTarget)
    {
        this->camTarget = inTarget;
        viewTransform.LookTo(camTarget);
    }

    void SetTranslation(const FVec3 inTranslation)
    {
        viewTransform.translation = inTranslation;
    }

    void Translate(FVec3 inTransDelta)
    {
        // inTransDelta is in camera's local, needs to be changed to world
        inTransDelta = viewTransform.rotation.RotateVector(inTransDelta);

        viewTransform.translation += inTransDelta;
    }

    void Rotate(FVec3 inRotDelta)
    {
        // inRotDelta is also in camera's local, but we need't to change it to world, because previous rotation will be applied first, making the coords in camera local
        // imagine putting a new camera from wolrd's origin to the position of old camera using old transform
        viewTransform.rotation = FQuat::FromEulerZYX(inRotDelta.x, inRotDelta.y, inRotDelta.z) * viewTransform.rotation;
    }

    FMat4x4 GetViewMatrix()
    {
        return viewTransform.GetViewMatrix();
    }

    FMat4x4 GetProjectionMatrix()
    {
        return rzProjection.GetProjectionMatrix();
    }

    bool Moving()
    {
        return keys.left || keys.right || keys.up || keys.down || keys.front || keys.back;
    }

    void MoveCamera(float deltaTime)
    {
        if (Moving()) {
            float frameMovement = moveSpeed * deltaTime;
            FVec3 posDelta = FVec3Consts::zero;
            if (keys.front)
                posDelta += FVec3(1.0f, .0f, .0f) * frameMovement;
            if (keys.back)
                posDelta -= FVec3(1.0f, .0f, .0f) * frameMovement;
            if (keys.left)
                posDelta -= FVec3(.0f, 1.0f, .0f) * frameMovement;
            if (keys.right)
                posDelta += FVec3(.0f, 1.0f, .0f) * frameMovement;
            if (keys.up)
                posDelta += FVec3(.0f, .0f, 1.0f) * frameMovement;
            if (keys.down)
                posDelta -= FVec3(.0f, .0f, 1.0f) * frameMovement;

            Translate(posDelta);
        }
    }

private:
    // vt makes camera from local to world
    FViewTransform viewTransform;
    FReversedZPerspectiveProjection rzProjection;


    FVec3 camTarget = FVec3Consts::unitZ;
};