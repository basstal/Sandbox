#include "Camera.hpp"

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(glm::vec3 inPosition = glm::vec3(0.0f), glm::vec3 inWorldUp = DEFAULT_UP, float inRotationX = DEFAULT_ROTATION_X, float inRotationZ = DEFAULT_ROTATION_Z) :
	front(DEFAULT_FRONT), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	position = inPosition;
	worldUp = inWorldUp;
	rotationX = inRotationX;
	rotationZ = inRotationZ;
	UpdateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float inRotationX, float inRotationZ) :
	front(DEFAULT_FRONT), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	position = glm::vec3(posX, posY, posZ);
	worldUp = glm::vec3(upX, upY, upZ);
	rotationX = inRotationX;
	rotationZ = inRotationZ;
	UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;
	if (direction == FORWARD)
		position += front * velocity;
	if (direction == BACKWARD)
		position -= front * velocity;
	if (direction == LEFT)
		position -= right * velocity;
	if (direction == RIGHT)
		position += right * velocity;
}

void Camera::CameraYawRotate(float delta)
{
	rotationX += delta;
	UpdateCameraVectors();
}

void Camera::CameraPitchRotate(float delta)
{
	rotationZ += delta;
	UpdateCameraVectors();
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	rotationX += yOffset;
	rotationZ += xOffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (rotationX > 89.0f)
			rotationX = 89.0f;
		if (rotationX < -89.0f)
			rotationX = -89.0f;
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
		position += deltaSpeed * front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= deltaSpeed * front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= deltaSpeed * right;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += deltaSpeed * right;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		position += deltaSpeed * worldUp;
}
void Camera::Reset()
{
	position = glm::vec3(0.0f);
	rotationX = DEFAULT_ROTATION_X;
	rotationZ = DEFAULT_ROTATION_Z;
	UpdateCameraVectors();
}


void Camera::UpdateCameraVectors()
{
	// Calculate the new Front vector
	front.x = sin(glm::radians(rotationZ)) * cos(glm::radians(rotationX));
	front.y = cos(glm::radians(rotationZ)) * cos(glm::radians(rotationX));
	front.z = sin(glm::radians(rotationX));
	front = glm::normalize(front);

	// Assuming WorldUp is glm::vec3(0, 0, 1) since Z is up
	right = glm::normalize(glm::cross(front, worldUp)); // Recalculate the Right vector
	up = glm::normalize(glm::cross(right, front)); // Recalculate the Up vector, it should be noted that cross product order is changed to maintain the right-hand rule
}
