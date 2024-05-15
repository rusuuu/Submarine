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
bool block = false;
bool lastMoveForward;

std::vector<glm::vec3> submarineInitialHitbox;
std::vector<glm::vec3> terrainInitialHitbox;

Camera* pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0, 0.0, 3.0));

bool VerifyHitbox(std::vector<glm::vec3> submarineHitbox, std::vector<glm::vec3> terrainHitbox)
{
	for (glm::vec3& submarineVertex: submarineHitbox)
		for (glm::vec3& terrainVertex : terrainHitbox)
		{
			float distance = glm::distance(submarineVertex, terrainVertex);
			if (distance < 0.5f)
			{
				std::cout << block << " " << lastMoveForward << " " << submarineAccel << "\n";
				return true; // Daca este, avem o coliziune
			}
		}
	block = false;
	return false;
}

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

glm::mat4 DetermineSubmarineTransfMatrix(float scaleFactor) {
	// ** MODEL **
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(submarineX, submarineY, submarineZ)); // Move to scene centre
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));	// Scale model
	model = glm::rotate(model, glm::radians(submarineAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	// ** MODEL **

	return model;
}

glm::mat4 DetermineTerrainTransfMatrix(float scaleFactor) {
	// ** MODEL **
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0)); // Move to scene centre
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));	// Scale model
	// ** MODEL **

	return model;
}

std::vector<glm::vec3> TransformHitbox(const std::vector<glm::vec3>& hitbox, const glm::mat4& transform) {
	std::vector<glm::vec3> transformedHitbox;
	transformedHitbox.reserve(hitbox.size());

	for (const auto& vertex : hitbox) {
		glm::vec4 transformedVertex = transform * glm::vec4(vertex, 1.0f);
		transformedHitbox.push_back(glm::vec3(transformedVertex));
	}

	return transformedHitbox;
}

void ProcessKeyboardMovement(ESubmarineMovementType direction, float deltaTime) {
	//float velocity = (float)(cameraSpeedFactor * deltaTime);
	int sgn;
	glm::mat4 submarineTransformMatrix;
	glm::mat4 terrainTransformMatrix;
	std::vector<glm::vec3> submarineHitbox;
	std::vector<glm::vec3> terrainHitbox;
	switch (direction) {
	case ESubmarineMovementType::MOVE:
		submarineTransformMatrix = DetermineSubmarineTransfMatrix(1.0f);
		submarineHitbox = TransformHitbox(submarineInitialHitbox, submarineTransformMatrix);
		terrainTransformMatrix = DetermineTerrainTransfMatrix(0.2f);
		terrainHitbox = TransformHitbox(terrainInitialHitbox, terrainTransformMatrix);
		if (!VerifyHitbox(submarineHitbox, terrainHitbox))
		{
			submarineZ -= submarineAccel * cos(glm::radians(submarineAngle));
			submarineX -= submarineAccel * sin(glm::radians(submarineAngle));
			propellerAngle -= 150 * submarineAccel;
		}
		else
		{
			if (submarineAccel > 0.0f && !block)
				lastMoveForward = true;
			if (submarineAccel < 0.0f && !block)
				lastMoveForward = false;
			if (!block)
			{
				block = true;
				submarineAccel = 0.0f;
			}
			if ((submarineAccel > 0.0f && !lastMoveForward) || (submarineAccel < 0.0f && lastMoveForward))
			{
				std::cout << "Do it?";
				submarineZ -= submarineAccel * cos(glm::radians(submarineAngle));
				submarineX -= submarineAccel * sin(glm::radians(submarineAngle));
				propellerAngle -= 150 * submarineAccel;
			}
		}
		break;
	case ESubmarineMovementType::MOVELEFT:
		if (submarineAccel > 0)
			sgn = 1;
		else
			sgn = -1;
		submarineAngle+=1.1*sqrtf(abs(submarineAccel))*sgn;
		break;
	case ESubmarineMovementType::MOVERIGHT:
		if (submarineAccel > 0)
			sgn = 1;
		else
			sgn = -1;
		submarineAngle-=1.1*sqrtf(abs(submarineAccel)) * sgn;
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
			submarineVerticalAccel += 0.0000375f;
		if (submarineVerticalAccel > 0.0f)
			submarineVerticalAccel -= 0.0000375f;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && (!block || (block && !lastMoveForward)))
			if (submarineAccel < 0.02f)
				submarineAccel += 0.00002f;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && (!block || (block && lastMoveForward)))
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