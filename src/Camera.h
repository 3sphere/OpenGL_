#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glfw3.h>

class Camera
{
public:
	Camera(float x, float y, float z, float yaw = -90.0f, float pitch = 0.0f);
	Camera(glm::vec3 pos, float yaw = -90.0f, float pitch = 0.0f);
	const glm::mat4 GetViewMatrix() const { return glm::lookAt(mPosition, mPosition + mFront, mUp); }
	void ProcessInput(GLFWwindow* window);
	void Update(float deltaTime);

	const glm::vec3& GetPosition() const { return mPosition; }
	const glm::vec3& GetFront() const { return mFront; }

private:
	void UpdateCameraVectors();

	void ProcessKeyboard(GLFWwindow* window);
	void ProcessMouse(GLFWwindow* window);

	glm::vec3 mPosition;
	glm::vec3 mFront;
	glm::vec3 mUp;
	glm::vec3 mRight;
	glm::vec3 mVelocity;

	float mYaw;
	float mPitch;

	float mMovementSpeed;
	float mMouseSensitivity;

	bool mFirstMouse;
	float mLastX;
	float mLastY;

	float mForwardSpeed;
	float mRightSpeed;
};