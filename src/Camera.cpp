#include "Camera.h"
#include <glfw3.h>

Camera::Camera(float x, float y, float z, float yaw, float pitch) :
	mPosition(glm::vec3(x, y, z)),
	mYaw(yaw),
	mPitch(pitch),
	mMovementSpeed(2.5f),
	mMouseSensitivity(0.05f),
	mFirstMouse(true),
	mLastX(0.0f),
	mLastY(0.0f),
	mForwardSpeed(0.0),
	mRightSpeed(0.0f)
{

}

Camera::Camera(glm::vec3 pos, float yaw, float pitch) :
	mPosition(pos),
	mYaw(yaw),
	mPitch(pitch),
	mMovementSpeed(2.5f),
	mMouseSensitivity(0.05f),
	mFirstMouse(true),
	mLastX(0.0f),
	mLastY(0.0f),
	mForwardSpeed(0.0),
	mRightSpeed(0.0f)
{
	UpdateCameraVectors();
}

void Camera::ProcessInput(GLFWwindow* window)
{
	ProcessKeyboard(window);
	ProcessMouse(window);
}

void Camera::ProcessKeyboard(GLFWwindow* window)
{
	mForwardSpeed = 0.0f;
	mRightSpeed = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		mForwardSpeed += mMovementSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		mForwardSpeed -= mMovementSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		mRightSpeed -= mMovementSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		mRightSpeed += mMovementSpeed;
}

void Camera::ProcessMouse(GLFWwindow* window)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (mFirstMouse)
	{
		mLastX = xpos;
		mLastY = ypos;
		mFirstMouse = false;
	}

	float xOffset = (float)xpos - mLastX;
	float yOffset = mLastY - (float)ypos;
	xOffset *= mMouseSensitivity;
	yOffset *= mMouseSensitivity;

	mLastX = xpos;
	mLastY = ypos;

	mYaw += xOffset;
	mPitch += yOffset;

	if (mPitch > 89.0f)
		mPitch = 89.0f;
	if (mPitch < -89.0f)
		mPitch = -89.0f;
}

void Camera::Update(float deltaTime)
{
	mPosition += mForwardSpeed * deltaTime * mFront + mRightSpeed * deltaTime * mRight;
	UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
	glm::vec3 front;
	front.x = cos(glm::radians(mPitch)) * cos(glm::radians(mYaw));
	front.y = sin(glm::radians(mPitch));
	front.z = cos(glm::radians(mPitch)) * sin(glm::radians(mYaw));
	mFront = glm::normalize(front);
	mRight = glm::normalize(glm::cross(mFront, worldUp));
	mUp = glm::normalize(glm::cross(mRight, mFront));
}