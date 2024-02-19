#include "Camera.hpp"

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) :
	Front(glm::vec3(0.0f, 1.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	UpdateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
	Front(glm::vec3(0.0f, 1.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction == FORWARD)
		Position += Front * velocity;
	if (direction == BACKWARD)
		Position -= Front * velocity;
	if (direction == LEFT)
		Position -= Right * velocity;
	if (direction == RIGHT)
		Position += Right * velocity;
}

void Camera::CameraYawRotate(float delta)
{
	Yaw += delta;
	UpdateCameraVectors();
}

void Camera::CameraPitchRotate(float delta)
{
	Pitch += delta;
	UpdateCameraVectors();
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
	xOffset *= MouseSensitivity;
	yOffset *= MouseSensitivity;

	Yaw += xOffset;
	Pitch += yOffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset)
{
	if (Zoom >= 1.0f && Zoom <= 45.0f)
		Zoom -= yOffset;
	if (Zoom <= 1.0f)
		Zoom = 1.0f;
	if (Zoom >= 45.0f)
		Zoom = 45.0f;
}

void Camera::UpdatePosition(float deltaTime, GLFWwindow* window)
{
	float deltaSpeed = MovementSpeed * deltaTime;
	// glm::vec3 moveFront = glm::vec3(cameraf.x, 0.0f, cameraFront.z);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Position += deltaSpeed * Front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Position -= deltaSpeed * Front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Position -= deltaSpeed * Right;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Position += deltaSpeed * Right;
}


void Camera::UpdateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.z = sin(glm::radians(Pitch));
	Front = glm::normalize(front);

	// Assuming WorldUp is glm::vec3(0, 0, 1) since Z is up
	Right = glm::normalize(glm::cross(Front, WorldUp)); // Recalculate the Right vector
	Up = glm::normalize(glm::cross(Right, Front)); // Recalculate the Up vector, it should be noted that cross product order is changed to maintain the right-hand rule
}
