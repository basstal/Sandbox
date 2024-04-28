#include "pch.hpp"

#include "Camera.hpp"

#include "Engine/EntityComponent/GameObject.hpp"
#include "Engine/PostProcess.hpp"
#include "Engine/RendererSource/PbrRendererSource.hpp"
#include "FileSystem/Directory.hpp"
#include "Generated/Camera.rfks.h"
#include "Misc/GlmExtensions.hpp"
#include "VulkanRHI/Common/ShaderSource.hpp"
#include "VulkanRHI/Renderer.hpp"

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
    postProcess = std::make_shared<PostProcess>();

    worldUp  = DEFAULT_WORLD_UP;
    worldUp1 = DEFAULT_WORLD_UP;
    onComponentCreate.Bind(
        [this](const std::shared_ptr<IComponent>& inComponent)
        {
            if (inComponent.get() == this)
            {
                UpdateCameraVectors();
            }
        });
    auto postProcessDirectory = Directory::GetAssetsDirectory().GetDirectory("Shaders/PostProcess");
    auto postProcessFiles     = postProcessDirectory.GetFiles();
    for (auto file : postProcessFiles)
    {
        std::shared_ptr<ShaderSource> postProcessSource = std::make_shared<ShaderSource>(file, "");
        postProcessFragShaders[postProcessSource]       = false;
    }
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

const rfk::Class* Sandbox::Camera::GetDerivedClass() { return rfk::classCast(rfk::getArchetype<Camera>()); }


Sandbox::Vector3 Sandbox::Camera::NdcToWorld(const float& x, const float& y, const float& z)
{
    glm::mat4 projectionMatrix        = GetProjectionMatrix();
    glm::vec4 clipPosition            = glm::vec4(x, y, z, 1.0);
    glm::mat4 inverseProjectionMatrix = glm::inverse(projectionMatrix);  // 假设projMatrix是当前摄像机的投影矩阵
    glm::vec4 viewPosition            = inverseProjectionMatrix * clipPosition;
    viewPosition /= viewPosition.w;
    LOGD("Engine", "viewPos {}", ToString(viewPosition))
    glm::mat4 viewMatrix        = GetViewMatrix();
    glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);  // 假设viewMatrix是当前摄像机的视图矩阵
    glm::vec4 worldPosition     = inverseViewMatrix * viewPosition;
    LOGD("Engine", "worldPos {}", ToString(worldPosition))
    return Vector3(worldPosition);
}

Sandbox::Vector3 Sandbox::Camera::NdcToWorld(const Vector3& ndcCoordinate) { return NdcToWorld(ndcCoordinate.x, ndcCoordinate.y, ndcCoordinate.z); }