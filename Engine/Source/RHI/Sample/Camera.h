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

    Camera(FVec3 inPos, FVec3 inRot, ProjectParams pParam)
        : camPosition(inPos),
        camRotation(inRot),
        rzProjection(pParam.fov, pParam.width, pParam.height, pParam.nearPlane, pParam.farPlane)
    {
        UpdateViewTransform();
    }

    void SetPosition(FVec3 inPosition)
    {
        this->camPosition = inPosition;
        UpdateViewTransform();
    }

    void Translate(FVec3 inTranslation)
    {
        this->camPosition += inTranslation;
        UpdateViewTransform();
    }

    void SetRotation(FVec3 inRotation)
    {
        this->camRotation = inRotation;
        UpdateViewTransform();
    }

    void Rotate(FVec3 inRotation)
    {
        this->camRotation += inRotation;
        UpdateViewTransform();
    }

    void SetTarget(FVec3 inTarget)
    {
        this->camTarget = inTarget;
        vt.LookTo(camTarget);
    }

    void SetTranslation(const FVec3 inDelta)
    {
        this->camPosition += inDelta;
        UpdateViewTransform();
    }

    FMat4x4 GetViewMatrix()
    {
        return vt.GetViewMatrix();
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
            FVec3 forward { 0.0f, 0.0f, 1.0f };
            FVec3 side { 1.0f, 0.0f, 0.0f };
            FVec3 up { 0.0f, 1.0f, 0.0f };

            float frameMovement = moveSpeed * deltaTime;
            if (keys.front)
                camPosition += forward * frameMovement;
            if (keys.back)
                camPosition -= forward * frameMovement;
            if (keys.left)
                camPosition -= side * frameMovement;
            if (keys.right)
                camPosition += side * frameMovement;
            if (keys.up)
                camPosition += up * frameMovement;
            if (keys.down)
                camPosition -= up * frameMovement;

            UpdateViewTransform();
        }
    }

private:
    FViewTransform vt;
    FReversedZPerspectiveProjection rzProjection;

    FVec3 camPosition = FVec3Consts::zero;
    FVec3 camTarget = FVec3Consts::unitZ;
    FVec3 camRotation = FVec3Consts::zero;

    void UpdateViewTransform()
    {
        vt.translation = this->camPosition;
        vt.rotation = FQuat::FromEulerZYX(this->camRotation.x, this->camRotation.y, this->camRotation.z);
    }
};