#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float DEFAULT_ROTATION_X = 0.0f;
const float DEFAULT_ROTATION_Z = 0.0f;
const glm::vec3 DEFAULT_FRONT = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 DEFAULT_UP = glm::vec3(0.0f, 0.0f, 1.0f);
const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 45.0f;

class Camera
{
public:
	// Camera Attributes
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	// Euler Angles
	float rotationX;
	float rotationZ;
	// Camera options
	float movementSpeed;
	float mouseSensitivity;
	float zoom;


	// Constructor with vectors
	Camera(glm::vec3 inPosition, glm::vec3 inWorldUp, float rotationX, float rotationZ);

	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float rotationX, float rotationZ);


	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix();


	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);


	void CameraYawRotate(float delta);

	void CameraPitchRotate(float delta);

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);


	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yOffset);

	void UpdatePosition(float deltaTime, GLFWwindow* window);

	void Reset();

private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void UpdateCameraVectors();
};
