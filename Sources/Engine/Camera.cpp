#include "pch.hpp"

#include "Generated/Camera.rfks.h"
#include "Misc/GlmExtensions.hpp"

/**
 * \brief 默认的相机速度
 */
constexpr float SPEED = 2.5f;
/**
 * \brief 默认的鼠标灵敏度
 */
constexpr float SENSITIVITY = 0.05f;

Sandbox::Camera::Camera(glm::vec3 inWorldUp, float inAspectRatio)
{
    // inProperty->aspectRatio = aspectRatio;
    aspectRatio = inAspectRatio;
    worldUp     = inWorldUp;
    worldUp1    = inWorldUp;
    // property = inProperty;
    UpdateCameraVectors();
}

glm::mat4 Sandbox::Camera::GetViewMatrix()
{
    auto eyePos = this->position.ToGlmVec3();
    // LOGD("eyePos : {}", Sandbox::ToString(eyePos))
    return glm::lookAt(eyePos, eyePos + front, up);
}

glm::mat4 Sandbox::Camera::GetProjectionMatrix() { return glm::perspective(glm::radians(this->fieldOfView), this->aspectRatio, this->nearPlane, this->farPlane); }

void Sandbox::Camera::ProcessKeyboard(ECameraMovement direction, float deltaTime)
{
    float         velocity = SPEED * deltaTime;
    const Vector3 directionMapping[ECameraMovement::MAX]{
        front1,  // FORWARD
        -front1,  // BACKWARD
        -right1,  // LEFT
        right1,  // RIGHT
        worldUp1,  // UP
    };
    this->position += directionMapping[direction] * velocity;
}

void Sandbox::Camera::CameraYawRotate(float delta)
{
    this->rotationX += delta;
    UpdateCameraVectors();
}

void Sandbox::Camera::CameraPitchRotate(float delta)
{
    this->rotationX += delta;
    UpdateCameraVectors();
}

void Sandbox::Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
    xOffset *= SENSITIVITY;
    yOffset *= SENSITIVITY;

    this->rotationX += yOffset;
    this->rotationZ += xOffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (this->rotationX > 89.0f)
        {
            this->rotationX = 89.0f;
        }
        if (this->rotationX < -89.0f)
        {
            this->rotationX = -89.0f;
        }
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

void Sandbox::Camera::Reset()
{
    position  = Vector3(0.0f);
    rotationX = 0.0f;
    rotationZ = 0.0f;
    UpdateCameraVectors();
}

void Sandbox::Camera::UpdateCameraVectors()
{
    auto rx    = glm::radians(this->rotationX);
    auto rz    = glm::radians(this->rotationZ);
    auto cosrx = cos(rx);
    // Calculate the new Front vector
    front.x = sin(rz) * cosrx;
    front.y = cos(rz) * cosrx;
    front.z = sin(rx);
    front   = glm::normalize(front);
    front1  = front;

    // Assuming WorldUp is glm::vec3(0, 0, 1) since Z is up
    right  = glm::normalize(glm::cross(front, worldUp));  // Recalculate the Right vector
    right1 = right;
    up     = glm::normalize(glm::cross(right, front));  // Recalculate the Up vector, it should be noted that cross product order is changed to maintain the right-hand rule
    up1 = up;
}
