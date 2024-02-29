#include "Camera.hpp"

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Infrastructures/DataBinding.hpp"

Camera::Camera(glm::vec3 inWorldUp, std::shared_ptr<CameraPersistence> inPersistence):
    front(DEFAULT_FRONT), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    this->worldUp = inWorldUp;
    this->persistence = inPersistence;
    UpdateCameraVectors();
}

Camera::Camera(glm::vec3 inPosition = glm::vec3(0.0f), glm::vec3 inWorldUp = DEFAULT_UP, float inRotationX = DEFAULT_ROTATION_X, float inRotationZ = DEFAULT_ROTATION_Z) :
    front(DEFAULT_FRONT), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    this->persistence = std::make_shared<CameraPersistence>();
    this->persistence->position = inPosition;
    worldUp = inWorldUp;
    this->persistence->rotationX = inRotationX;
    this->persistence->rotationZ = inRotationZ;
    UpdateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float inRotationX, float inRotationZ) :
    front(DEFAULT_FRONT), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    this->persistence = std::make_shared<CameraPersistence>();
    this->persistence->position = glm::vec3(posX, posY, posZ);
    worldUp = glm::vec3(upX, upY, upZ);
    this->persistence->rotationX = inRotationX;
    this->persistence->rotationZ = inRotationZ;
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(this->persistence->position, this->persistence->position + front, up);
}

glm::mat4 Camera::GetProjectionMatrix()
{
    return glm::perspective(glm::radians(persistence->fieldOfView), persistence->aspectRatio, persistence->nearPlane, persistence->farPlane);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)
        this->persistence->position += front * velocity;
    if (direction == BACKWARD)
        this->persistence->position -= front * velocity;
    if (direction == LEFT)
        this->persistence->position -= right * velocity;
    if (direction == RIGHT)
        this->persistence->position += right * velocity;
}

void Camera::CameraYawRotate(float delta)
{
    this->persistence->rotationX += delta;
    UpdateCameraVectors();
}

void Camera::CameraPitchRotate(float delta)
{
    this->persistence->rotationZ += delta;
    UpdateCameraVectors();
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    this->persistence->rotationX += yOffset;
    this->persistence->rotationZ += xOffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (this->persistence->rotationX > 89.0f)
            this->persistence->rotationX = 89.0f;
        if (this->persistence->rotationX < -89.0f)
            this->persistence->rotationX = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset)
{
    if (zoom >= 1.0f && zoom <= 45.0f)
        zoom -= yOffset;
    if (zoom <= 1.0f)
        zoom = 1.0f;
    if (zoom >= 45.0f)
        zoom = 45.0f;
}

void Camera::UpdatePosition(float deltaTime, GLFWwindow* window)
{
    float deltaSpeed = movementSpeed * deltaTime;
    // glm::vec3 moveFront = glm::vec3(cameraf.x, 0.0f, cameraFront.z);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        persistence->position += deltaSpeed * front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        persistence->position -= deltaSpeed * front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        persistence->position -= deltaSpeed * right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        persistence->position += deltaSpeed * right;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        persistence->position += deltaSpeed * worldUp;
}

void Camera::Reset()
{
    persistence->position = glm::vec3(0.0f);
    persistence->rotationX = DEFAULT_ROTATION_X;
    persistence->rotationZ = DEFAULT_ROTATION_Z;
    UpdateCameraVectors();
}


void Camera::UpdateCameraVectors()
{
    auto rotationX = this->persistence->rotationX;
    auto rotationZ = this->persistence->rotationZ;
    // Calculate the new Front vector
    front.x = sin(glm::radians(rotationZ)) * cos(glm::radians(rotationX));
    front.y = cos(glm::radians(rotationZ)) * cos(glm::radians(rotationX));
    front.z = sin(glm::radians(rotationX));
    front = glm::normalize(front);

    // Assuming WorldUp is glm::vec3(0, 0, 1) since Z is up
    right = glm::normalize(glm::cross(front, worldUp)); // Recalculate the Right vector
    up = glm::normalize(glm::cross(right, front)); // Recalculate the Up vector, it should be noted that cross product order is changed to maintain the right-hand rule
}
