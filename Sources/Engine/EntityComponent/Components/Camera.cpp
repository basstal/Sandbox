#include "pch.hpp"

#include "Camera.hpp"

#include "Engine/EntityComponent/GameObject.hpp"
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

const glm::vec3 DEFAULT_WORLD_UP = glm::vec3(0.0f, 0.0f, 1.0f);


Sandbox::Camera::Camera()
{
    worldUp  = DEFAULT_WORLD_UP;
    worldUp1 = DEFAULT_WORLD_UP;
    UpdateCameraVectors();
}

Sandbox::Camera::Camera(float inAspectRatio)
{
    // inProperty->aspectRatio = aspectRatio;
    aspectRatio = inAspectRatio;
    worldUp     = DEFAULT_WORLD_UP;
    worldUp1    = DEFAULT_WORLD_UP;
    // property = inProperty;
    UpdateCameraVectors();
}

glm::mat4 Sandbox::Camera::GetViewMatrix()
{
    auto gameObject = this->gameObject.lock();
    auto eyePos     = gameObject->transform->position.ToGlmVec3();
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
    auto gameObject = this->gameObject.lock();
    gameObject->transform->position += directionMapping[direction] * velocity;
    // this->position += directionMapping[direction] * velocity;
}

void Sandbox::Camera::CameraYawRotate(float delta)
{
    this->rotationX += delta;
    // auto gameObject = this->gameObject.lock();
    // gameObject->transform->rotation *= glm::quat(glm::radians(glm::vec3(delta, 0.0f, 0.0f)));
    UpdateCameraVectors();
}

void Sandbox::Camera::CameraPitchRotate(float delta)
{
    this->rotationX += delta;
    // auto gameObject = this->gameObject.lock();
    // gameObject->transform->rotation *= glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, delta)));
    UpdateCameraVectors();
}

void Sandbox::Camera::ProcessMouseMovement(float xOffset, float yOffset)
{
    xOffset *= SENSITIVITY;
    yOffset *= SENSITIVITY;

    this->rotationX += yOffset;
    this->rotationZ += xOffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    rotationX = std::max(-89.0f, std::min(89.0f, rotationX));
    // auto gameObject = this->gameObject.lock();
    //     gameObject->transform->rotation *= glm::quat(glm::radians(glm::vec3(yOffset, 0.0f, xOffset)));
    //     auto degrees = gameObject->transform->rotation.GetEulerDegrees();
    //
    //     // Make sure that when pitch is out of bounds, screen doesn't get flipped , always constrain Pitch
    //     degrees.x                       = std::max(-89.0f, std::min(89.0f, degrees.x));
    //     gameObject->transform->rotation = glm::quat(glm::radians(degrees));

    // Update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

void Sandbox::Camera::Reset()
{
    // position  = Vector3(0.0f);
    rotationX                       = 0.0f;
    rotationZ                       = 0.0f;
    auto gameObject                 = this->gameObject.lock();
    gameObject->transform->position = glm::vec3(0.0f);
    gameObject->transform->rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    UpdateCameraVectors();
}

void Sandbox::Camera::UpdateCameraVectors()
{
    auto rx = glm::radians(this->rotationX);
    auto rz = glm::radians(this->rotationZ);
    // auto rx         = 0.0f;
    // auto rz         = 0.0f;
    // auto gameObject = this->gameObject.lock();
    // if (gameObject != nullptr)
    // {
    //     glm::vec3 eulerRadians = gameObject->transform->rotation.GetEulerRadians();
    //     rx                     = eulerRadians.x;
    //     rz                     = eulerRadians.z;
    // }
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
    up1    = up;
}

std::string Sandbox::Camera::GetDerivedClassName() { return getArchetype().getName(); }

const rfk::Class* Sandbox::Camera::GetDerivedClass() { return rfk::classCast(rfk::getArchetype<Camera>());}