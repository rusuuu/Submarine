#pragma once

#include <glfw3.h>

#include "Camera.h"

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const unsigned int SHADOW_WIDTH = 4096;
const unsigned int SHADOW_HEIGHT = 4096;

// timing
double deltaTime = 0.0f;    // time between current frame and last frame
double lastFrame = 0.0f;

float submarineX = 0.0f;
float submarineY = 0.0f;
float submarineZ = 0.0f;
float submarineAngle = 0.0f;
float submarineAccel = 0.0f;
float submarineVerticalAccel = 0.0f;
float propellerAngle = 0.0f;

Camera* pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0, 0.0, 3.0));

void processInput(GLFWwindow* window) {	// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		pCamera->ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		pCamera->ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		pCamera->ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		pCamera->ProcessKeyboard(RIGHT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		pCamera->ProcessKeyboard(UP, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		pCamera->ProcessKeyboard(DOWN, (float)deltaTime);

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		pCamera->Reset(width, height);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	pCamera->Reshape(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	pCamera->MouseControl((float)xpos, (float)ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yOffset) {
	pCamera->ProcessMouseScroll((float)yOffset);
}

// might not be necessary
bool RotateLight = true;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		RotateLight = true;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		RotateLight = false;
	}
}

enum ESubmarineMovementType
{
	MOVEUNKNOWN,
	MOVE,
	MOVELEFT,
	MOVERIGHT,
	MOVEVERTICAL
};

void ProcessKeyboardMovement(ESubmarineMovementType direction, float deltaTime) {
	//float velocity = (float)(cameraSpeedFactor * deltaTime);
	switch (direction) {
	case ESubmarineMovementType::MOVE:
		submarineZ -= submarineAccel * cos(glm::radians(submarineAngle));
		submarineX -= submarineAccel * sin(glm::radians(submarineAngle));
		propellerAngle -= 150*submarineAccel;
		break;
	case ESubmarineMovementType::MOVELEFT:
		submarineAngle+=1.1*sqrtf(abs(submarineAccel));
		break;
	case ESubmarineMovementType::MOVERIGHT:
		submarineAngle-=1.1*sqrtf(abs(submarineAccel));
		break;
	case ESubmarineMovementType::MOVEVERTICAL:
		submarineY += submarineVerticalAccel;
		break;
	}
}

void processSubmarineMovement(GLFWwindow* window) {	// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && submarineY < 1.39 && submarineVerticalAccel < 0.0075)
			submarineVerticalAccel += 0.000075;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && submarineVerticalAccel > -0.0075)
			submarineVerticalAccel -= 0.000075;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && submarineY >= 1.39)
			submarineVerticalAccel = 0;
	}
	else
	{
		if (submarineVerticalAccel < 0.0f)
			submarineVerticalAccel += 0.00005f;
		if (submarineVerticalAccel > 0.0f)
			submarineVerticalAccel -= 0.00005f;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			if (submarineAccel < 0.02f)
				submarineAccel += 0.00002f;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			if (submarineAccel > -0.02f)
				submarineAccel -= 0.00002f;
	}
	else
	{
		if (submarineAccel < 0.0f)
			submarineAccel += 0.00001f;
		if (submarineAccel > 0.0f)
			submarineAccel -= 0.00001f;
	}
	ProcessKeyboardMovement(MOVE, deltaTime);
	ProcessKeyboardMovement(MOVEVERTICAL, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		ProcessKeyboardMovement(MOVELEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		ProcessKeyboardMovement(MOVERIGHT, deltaTime);

}