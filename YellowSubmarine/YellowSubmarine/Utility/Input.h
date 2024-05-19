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
float submarineVerticalAngle = 0.0f;
float submarineHorizontalAngle = 0.0f;
float submarineAccel = 0.0f;
float submarineVerticalAccel = 0.0f;
float propellerAngle = 0.0f;

std::vector<glm::vec3> submarineInitialHitbox;
std::vector<glm::vec3> terrainInitialHitbox;

Camera* pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0, 0.0, 3.0));

struct Face {
	glm::vec3 vertices[3];
	glm::vec3 normal;
};

glm::vec3 projectPointOnPlane(const glm::vec3& point, const Face& face) {
	float distance = glm::dot(point - face.vertices[0], face.normal);
	return point - distance * face.normal;
}

bool isPointInTriangle(const glm::vec3& point, const Face& face) {
	for (int i = 0; i < 3; ++i) {
		glm::vec3 edge = face.vertices[(i + 1) % 3] - face.vertices[i];
		glm::vec3 toPoint = point - face.vertices[i];
		glm::vec3 crossProduct = glm::cross(edge, toPoint);
		if (glm::dot(crossProduct, face.normal) < 0) {
			return false;
		}
	}
	return true;
}

bool detectCollision(const std::vector<glm::vec3> submarineHitbox, const std::vector<Face>& faces) {
	for (const auto& face : faces) {
		for (const auto& vertex : submarineHitbox) {
			glm::vec3 projectedPoint = projectPointOnPlane(vertex, face);
			if (isPointInTriangle(projectedPoint, face)) {
				float distance = glm::length(projectedPoint - vertex);
				if (distance < 0.02f)
				{
					if (submarineVerticalAccel < 0)
						submarineY += (0.02005f - distance);
					if (submarineVerticalAccel > 0)
						submarineY -= (0.02005f - distance);
					if (submarineAccel > 0)
					{
						submarineZ += (0.02005f - distance) * cos(glm::radians(submarineAngle));
						submarineX += (0.02005f - distance) * sin(glm::radians(submarineAngle));
					}
					if (submarineAccel < 0)
					{
						submarineZ += (-0.02005f + distance) * cos(glm::radians(submarineAngle));
						submarineX += (-0.02005f + distance) * sin(glm::radians(submarineAngle));
					}
					return true; // Daca este, avem o coliziune
				}
			}
		}
	}

	return false; // Nicio coliziune
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
	model = glm::rotate(model, glm::radians(submarineVerticalAngle), glm::vec3(1.0f, 0.0f, 0.0f));
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

void ProcessKeyboardMovement(ESubmarineMovementType direction, float deltaTime, const std::vector<Face>& faces) {
	//float velocity = (float)(cameraSpeedFactor * deltaTime);
	int sgn;
	glm::mat4 submarineTransformMatrix;
	std::vector<glm::vec3> submarineHitbox;
	switch (direction) {
	case ESubmarineMovementType::MOVE:
		submarineTransformMatrix = DetermineSubmarineTransfMatrix(1.0f);
		submarineHitbox = TransformHitbox(submarineInitialHitbox, submarineTransformMatrix);
		if (!detectCollision(submarineHitbox, faces))
		{
			submarineZ -= submarineAccel * cos(glm::radians(submarineAngle));
			submarineX -= submarineAccel * sin(glm::radians(submarineAngle));
			propellerAngle -= 150 * submarineAccel;
		}
		else
		{
			submarineAccel = 0;
			submarineVerticalAccel = 0;
			submarineVerticalAngle = 0;
		}
		break;
	case ESubmarineMovementType::MOVELEFT:
		if (submarineAccel > 0)
			sgn = 1;
		else
			sgn = -1;
		if (submarineHorizontalAngle < 30)
			submarineHorizontalAngle += 0.2;
		submarineAngle += 1.1 * sqrtf(abs(submarineAccel)) * sgn;
		break;
	case ESubmarineMovementType::MOVERIGHT:
		if (submarineAccel > 0)
			sgn = 1;
		else
			sgn = -1;
		if (submarineHorizontalAngle > -30)
			submarineHorizontalAngle -= 0.2;
		submarineAngle -= 1.1 * sqrtf(abs(submarineAccel)) * sgn;
		break;
	case ESubmarineMovementType::MOVEVERTICAL:
		submarineTransformMatrix = DetermineSubmarineTransfMatrix(1.0f);
		submarineHitbox = TransformHitbox(submarineInitialHitbox, submarineTransformMatrix);
		if (!detectCollision(submarineHitbox, faces))
		{
			if (submarineY >= 1.39)
				submarineVerticalAccel -= 0.0001;
			submarineY += submarineVerticalAccel;
			submarineVerticalAngle = 2000 * submarineVerticalAccel;
			propellerAngle -= abs(150 * submarineVerticalAccel);
		}
		else
		{
			submarineAccel = 0;
			submarineVerticalAccel = 0;
			submarineVerticalAngle = 0;
		}
		break;
	}
}

void processSubmarineMovement(GLFWwindow* window, const std::vector<Face>& faces) {	// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && submarineY < 1.39 && submarineVerticalAccel < 0.0075)
			submarineVerticalAccel += 0.000075;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && submarineY < 1.39 && submarineVerticalAccel > -0.0075)
			submarineVerticalAccel -= 0.000075;
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
	ProcessKeyboardMovement(MOVE, deltaTime, faces);
	ProcessKeyboardMovement(MOVEVERTICAL, deltaTime, faces);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS )
	{
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && (submarineAccel > 0.0001f || submarineAccel < -0.0001f))
			ProcessKeyboardMovement(MOVELEFT, deltaTime, faces);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && (submarineAccel > 0.0001f || submarineAccel < -0.0001f))
			ProcessKeyboardMovement(MOVERIGHT, deltaTime, faces);
		if (!(submarineAccel > 0.0001f || submarineAccel < -0.0001f))
		{
			if (submarineHorizontalAngle > 0)
				submarineHorizontalAngle -= 0.2;
			if (submarineHorizontalAngle < 0)
				submarineHorizontalAngle += 0.2;
		}
	}
	else
	{
		if (submarineHorizontalAngle > 0)
			submarineHorizontalAngle -= 0.2;
		if (submarineHorizontalAngle < 0)
			submarineHorizontalAngle += 0.2;
	}

}