//
// Created by 兰俊康 on 2023/3/14.
//
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    enum CameraType { lookAt, firstPerson };
    CameraType type = CameraType::lookAt;

    glm::vec3 rotation = glm::vec3();
    glm::vec3 position = glm::vec3();
    glm::vec3 viewPos  = glm::vec3();

    float rotationSpeed = 1.0f;
    float movementSpeed = 1.0f;

    bool updated = false;
    bool flipY = false;

    glm::mat4 perspective;
    glm::mat4 view;

    struct {
        bool left = false;
        bool right = false;
        bool up = false;
        bool down = false;
    } keys;

    bool moving()
    {
        return keys.left || keys.right || keys.up || keys.down;
    }

    float getNearClip() {
        return znear;
    }

    float getFarClip() {
        return zfar;
    }

    void setPerspective(float inFov, float inAspect, float inZnear, float inZfar)
    {
        this->fov = inFov;
        this->znear = inZnear;
        this->zfar = inZfar;
        perspective = glm::perspective(glm::radians(fov), inAspect, znear, zfar);
        if (flipY) {
            perspective[1][1] *= -1.0f;
        }
    };

    void updateAspectRatio(float aspect)
    {
        perspective = glm::perspective(glm::radians(fov), aspect, znear, zfar);
        if (flipY) {
            perspective[1][1] *= -1.0f;
        }
    }

    void setPosition(glm::vec3 inPosition)
    {
        this->position = inPosition;
        UpdateViewMatrix();
    }

    void setRotation(glm::vec3 inRotation)
    {
        this->rotation = inRotation;
        UpdateViewMatrix();
    }

    void rotate(glm::vec3 delta)
    {
        this->rotation += delta;
        UpdateViewMatrix();
    }

    void setTranslation(glm::vec3 translation)
    {
        this->position = translation;
        UpdateViewMatrix();
    };

    void translate(glm::vec3 delta)
    {
        this->position += delta;
        UpdateViewMatrix();
    }

    void setRotationSpeed(float inRotationSpeed)
    {
        this->rotationSpeed = inRotationSpeed;
    }

    void setMovementSpeed(float inMovementSpeed)
    {
        this->movementSpeed = inMovementSpeed;
    }

    void update(float deltaTime)
    {
        updated = false;
        if (type == CameraType::firstPerson)
        {
            if (moving())
            {
                glm::vec3 camFront;
                camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
                camFront.y = sin(glm::radians(rotation.x));
                camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
                camFront = glm::normalize(camFront);

                float moveSpeed = deltaTime * movementSpeed;

                if (keys.up)
                    position += camFront * moveSpeed;
                if (keys.down)
                    position -= camFront * moveSpeed;
                if (keys.left)
                    position -= glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
                if (keys.right)
                    position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
            }
        }
        UpdateViewMatrix();
    };

    // Update camera passing separate axis data (gamepad)
    // Returns true if view or position has been changed
    bool updatePad(glm::vec2 axisLeft, glm::vec2 axisRight, float deltaTime)
    {
        bool retVal = false;

        if (type == CameraType::firstPerson)
        {
            // Use the common console thumbstick layout
            // Left = view, right = move

            const float deadZone = 0.0015f;
            const float range = 1.0f - deadZone;

            glm::vec3 camFront;
            camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
            camFront.y = sin(glm::radians(rotation.x));
            camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
            camFront = glm::normalize(camFront);

            float moveSpeed = deltaTime * movementSpeed * 2.0f;
            float rotSpeed = deltaTime * rotationSpeed * 50.0f;

            // Move
            if (fabsf(axisLeft.y) > deadZone)
            {
                float pos = (fabsf(axisLeft.y) - deadZone) / range;
                position -= camFront * pos * ((axisLeft.y < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
                retVal = true;
            }
            if (fabsf(axisLeft.x) > deadZone)
            {
                float pos = (fabsf(axisLeft.x) - deadZone) / range;
                position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * pos * ((axisLeft.x < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
                retVal = true;
            }

            // Rotate
            if (fabsf(axisRight.x) > deadZone)
            {
                float pos = (fabsf(axisRight.x) - deadZone) / range;
                rotation.y += pos * ((axisRight.x < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
                retVal = true;
            }
            if (fabsf(axisRight.y) > deadZone)
            {
                float pos = (fabsf(axisRight.y) - deadZone) / range;
                rotation.x -= pos * ((axisRight.y < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
                retVal = true;
            }
        }
        else
        {
            // todo: move code from example base class for look-at
        }

        if (retVal)
        {
            UpdateViewMatrix();
        }

        return retVal;
    }
private:
    void UpdateViewMatrix()
    {
        glm::mat4 rotM = glm::mat4(1.0f);
        glm::mat4 transM;

        rotM = glm::rotate(rotM, glm::radians(rotation.x * (flipY ? -1.0f : 1.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
        rotM = glm::rotate(rotM, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotM = glm::rotate(rotM, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::vec3 translation = position;
        if (flipY) {
            translation.y *= -1.0f;
        }
        transM = glm::translate(glm::mat4(1.0f), translation);

        if (type == CameraType::firstPerson)
        {
            view = rotM * transM;
        }
        else
        {
            view = transM * rotM;
        }

        viewPos = glm::vec4(position, 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);

        updated = true;
    }

    float fov;
    float znear, zfar;
};
