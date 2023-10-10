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
        vt.MoveAndLookTo(camPosition, camTarget);
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
            FVec3 camFront;
            camFront.x = -cos(Angle2Radiance(camRotation.x)) * sin(Angle2Radiance(camRotation.y));
            camFront.y = sin(Angle2Radiance(camRotation.x));
            camFront.z = cos(Angle2Radiance(camRotation.x)) * cos(Angle2Radiance(camRotation.y));
            camFront.Normalize();

            FVec3 camRight = camFront.Cross(FVec3(0.0f, 1.0f, 0.0f)).Normalized();

            FVec3 camUp = camFront.Cross(camRight).Normalized();

            float frameMovement = moveSpeed * deltaTime;
            if (keys.front)
                camPosition += camFront * frameMovement;
            if (keys.back)
                camPosition -= camFront * frameMovement;
            if (keys.left)
                camPosition -= camRight * frameMovement;
            if (keys.right)
                camPosition += camRight * frameMovement;
            if (keys.up)
                camPosition += camUp * frameMovement;
            if (keys.down)
                camPosition -= camUp * frameMovement;

            UpdateViewTransform();
        }
    }

private:
    FViewTransform vt;
    FReversedZPerspectiveProjection rzProjection;

    FVec3 camPosition = FVec3Consts::zero;
    FVec3 camTarget = FVec3Consts::unitZ;
    FVec3 camRotation = FVec3Consts::zero;

    static float Angle2Radiance(float inAngle)
    {
        FAngle angle(inAngle);
        return angle.ToRadian();
    }

    void UpdateViewTransform()
    {
        vt.translation = this->camPosition;
        vt.rotation = FQuat::FromEulerZYX(this->camRotation.x, this->camRotation.y, this->camRotation.z);
    }
};