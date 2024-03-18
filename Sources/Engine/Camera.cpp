#include "Camera.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

/**
 * \brief 默认的相机速度
 */
constexpr float SPEED = 2.5f;
/**
 * \brief 默认的鼠标灵敏度
 */
constexpr float SENSITIVITY = 0.05f;

Sandbox::Camera::Camera(glm::vec3 inWorldUp, float aspectRatio, std::shared_ptr<CameraProperty> inProperty)
{
    inProperty->aspectRatio = aspectRatio;
    worldUp = inWorldUp;
    property = inProperty;
    UpdateCameraVectors();
}

glm::mat4 Sandbox::Camera::GetViewMatrix()
{
    return glm::lookAt(this->property->position, this->property->position + front, up);
}

glm::mat4 Sandbox::Camera::GetProjectionMatrix()
{
    return glm::perspective(glm::radians(this->property->fieldOfView), this->property->aspectRatio, this->property->nearPlane, this->property->farPlane);
}

void Sandbox::Camera::ProcessKeyboard(ECameraMovement direction, float deltaTime)
{
    float velocity = SPEED * deltaTime;
    const glm::vec3 directionMapping[ECameraMovement::MAX]
    {
        front, // FORWARD
        -front, // BACKWARD
        -right, // LEFT
        right, // RIGHT
        worldUp, // UP
    };
    this->property->position += directionMapping[direction] * velocity;
}

void Sandbox::Camera::CameraYawRotate(float delta)
{
    this->property->rotationX += delta;
    UpdateCameraVectors();
}

void Sandbox::Camera::CameraPitchRotate(float delta)
{
    this->property->rotationX += delta;
    UpdateCameraVectors();
}

void Sandbox::Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
    xOffset *= SENSITIVITY;
    yOffset *= SENSITIVITY;

    this->property->rotationX += yOffset;
    this->property->rotationZ += xOffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (this->property->rotationX > 89.0f)
        {
            this->property->rotationX = 89.0f;
        }
        if (this->property->rotationX < -89.0f)
        {
            this->property->rotationX = -89.0f;
        }
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

void Sandbox::Camera::Reset()
{
    property->position = glm::vec3(0.0f);
    property->rotationX = 0.0f;
    property->rotationZ = 0.0f;
    UpdateCameraVectors();
}

void Sandbox::Camera::UpdateCameraVectors()
{
    auto rotationX = this->property->rotationX;
    auto rotationZ = this->property->rotationZ;
    // Calculate the new Front vector
    front.x = sin(glm::radians(rotationZ)) * cos(glm::radians(rotationX));
    front.y = cos(glm::radians(rotationZ)) * cos(glm::radians(rotationX));
    front.z = sin(glm::radians(rotationX));
    front = glm::normalize(front);

    // Assuming WorldUp is glm::vec3(0, 0, 1) since Z is up
    right = glm::normalize(glm::cross(front, worldUp)); // Recalculate the Right vector
    up = glm::normalize(glm::cross(right, front)); // Recalculate the Up vector, it should be noted that cross product order is changed to maintain the right-hand rule
}
