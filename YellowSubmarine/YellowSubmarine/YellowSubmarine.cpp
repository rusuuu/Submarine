﻿// YellowSubmarine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdlib.h> // necesare pentru citirea shaderStencilTesting-elor
#include <irrKlang.h>

#include <GL/glew.h>

#define GLM_FORCE_CTOR_INIT
#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glfw3.h>

#include "Utility/Camera.h"
#include "Utility/Model.h"
#include "Utility/Shader.h"
#include "Utility/Input.h"
#include "Utility/Paths.h"

#include "Skybox.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <chrono>

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "assimp-vc140-mt.lib")
#pragma comment (lib, "irrKlang.lib")

glm::vec3 goldLightPos1(-0.475f, 0.87f, -34.0f);
glm::vec3 goldLightPos2(-27.5f, -2.5f, -16.5f);
glm::vec3 goldLightPos3(-3.75f, -2.59f, 4.45f);
glm::vec3 goldLightPos4(-45.0f, 0.92f, 15.5f);
glm::vec3 goldLightPos5(-22.0f, -2.55f, 2.0f);
glm::vec3 goldLightPos6(2.5f, 0.45f, 2.0f);

bool DrawSkybox(Shader shaderSkybox, glm::mat4& view, glm::mat4& projection) {
	// ** SKYBOX **

	// cubes
	glBindVertexArray(cubeMapVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	// draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	shaderSkybox.Use();

	auto currentTime = std::chrono::system_clock::now();
	auto currentTimeSeconds = std::chrono::time_point_cast<std::chrono::seconds>(currentTime);
	auto currentTimeSecondsToInt = currentTimeSeconds.time_since_epoch().count();

	int ambientValue;
	ambientValue = currentTimeSecondsToInt % 86400;
	int segment = ambientValue / 5400;
	float Ka;

	if (segment <= 7) {
		Ka = 0.2 + 0.1 * segment;
	}
	else if (segment <= 15) {
		Ka = 0.8 - 0.1 * (segment - 7);
	}
	else {
		Ka = 0.1;
	}

	shaderSkybox.SetFloat("Ka", Ka);

	view = glm::mat4(glm::mat3(pCamera->GetViewMatrix())); // remove translation from the view matrix

	shaderSkybox.SetMat4("view", view);
	shaderSkybox.SetMat4("projection", projection);
	shaderSkybox.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);

	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
	// ** SKYBOX **

	return true;
}

bool DrawObject(Shader shaderModel, Model objectModel, glm::mat4& view, glm::mat4& projection, glm::vec3 sunlightPos, glm::vec3 moonlightPos, float scaleFactor) {
	// ** MODEL **
	shaderModel.Use();

	auto currentTime = std::chrono::system_clock::now();
	auto currentTimeSeconds = std::chrono::time_point_cast<std::chrono::seconds>(currentTime);
	auto currentTimeSecondsToInt = currentTimeSeconds.time_since_epoch().count();

	int ambientValue;
	ambientValue = currentTimeSecondsToInt % 86400;
	int segment = ambientValue / 5400;
	float Ka;

	if (segment <= 7) {
		Ka = 0.2 + 0.1 * segment;
	}
	else if (segment <= 15) {
		Ka = 0.8 - 0.1 * (segment - 7);
	}
	else {
		Ka = 0.1;
	}
	view = pCamera->GetViewMatrix();

	shaderModel.SetFloat("Ka", Ka);

	shaderModel.SetMat4("view", view);
	shaderModel.SetMat4("projection", projection);

	shaderModel.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shaderModel.SetVec3("sunlightPos", sunlightPos);
	shaderModel.SetVec3("moonlightPos", moonlightPos);
	shaderModel.SetVec3("viewPos", pCamera->GetPosition());

	// Draw the loaded model
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Move to scene centre
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));	// Scale model
	//model = glm::rotate(model, degrees, glm::vec3(0.0f, 1.0f, 0.0f));
	shaderModel.SetMat4("model", model);
	objectModel.Draw(shaderModel);
	// ** MODEL **

	return true;
}

bool DrawPropeller(Shader shaderModel, Model objectModel, glm::mat4& view, glm::mat4& projection, float scaleFactor, float yPos) {
	// ** MODEL **
	shaderModel.Use();

	view = pCamera->GetViewMatrix();

	shaderModel.SetMat4("view", view);
	shaderModel.SetMat4("projection", projection);

	glm::vec3 relativePosition(0.0f, yPos, 1.56f);
	glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), glm::radians(submarineVerticalAngle), glm::vec3(1, 0, 0));
	glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), glm::radians(submarineAngle), glm::vec3(0, 1, 0));
	glm::mat4 rotationMatrixZ = glm::rotate(glm::mat4(1.0f), glm::radians(submarineHorizontalAngle), glm::vec3(0, 0, 1));
	glm::mat4 rotationMatrix = rotationMatrixY * rotationMatrixX * rotationMatrixZ;
	glm::vec4 rotatedPosition = rotationMatrix * glm::vec4(relativePosition, 1.0f);

	// Draw the loaded model
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(submarineX, submarineY, submarineZ) + glm::vec3(rotatedPosition)); // Move to scene centre
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));	// Scale model
	model = glm::rotate(model, glm::radians(submarineAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(propellerAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	shaderModel.SetMat4("model", model);
	objectModel.Draw(shaderModel);
	// ** MODEL **

	return true;
}

bool DrawFlaps(Shader shaderModel, Model objectModel, glm::mat4& view, glm::mat4& projection, float scaleFactor, float xPos, float angle) {
	// ** MODEL **
	shaderModel.Use();

	view = pCamera->GetViewMatrix();

	shaderModel.SetMat4("view", view);
	shaderModel.SetMat4("projection", projection);

	glm::vec3 relativePosition(xPos, 0.205f, 1.52f);
	glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), glm::radians(submarineVerticalAngle), glm::vec3(1, 0, 0));
	glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), glm::radians(submarineAngle), glm::vec3(0, 1, 0));
	glm::mat4 rotationMatrixZ = glm::rotate(glm::mat4(1.0f), glm::radians(submarineHorizontalAngle), glm::vec3(0, 0, 1));
	glm::mat4 rotationMatrix = rotationMatrixY * rotationMatrixX * rotationMatrixZ;
	glm::vec4 rotatedPosition = rotationMatrix * glm::vec4(relativePosition, 1.0f);

	// Draw the loaded model
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(submarineX, submarineY, submarineZ) + glm::vec3(rotatedPosition)); // Move to scene centre
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));	// Scale model
	model = glm::rotate(model, glm::radians(submarineAngle + angle), glm::vec3(0.0f, 1.0f, 0.0f));
	if (angle == 0)
	{
		model = glm::rotate(model, glm::radians(-submarineVerticalAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(submarineHorizontalAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else
	{
		model = glm::rotate(model, glm::radians(submarineVerticalAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-submarineHorizontalAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	model = glm::rotate(model, glm::radians(-submarineHorizontalAngle / 2), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-submarineHorizontalAngle / 2), glm::vec3(1.0f, 0.0f, 0.0f));
	shaderModel.SetMat4("model", model);
	objectModel.Draw(shaderModel);
	// ** MODEL **

	return true;
}

bool DrawHublou(Shader shaderModel, Model objectModel, glm::mat4& view, glm::mat4& projection, float scaleFactor) {
	// ** MODEL **
	shaderModel.Use();

	view = pCamera->GetViewMatrix();

	shaderModel.SetMat4("view", view);
	shaderModel.SetMat4("projection", projection);

	glm::vec3 relativePosition(0.0f, 0.2, -0.22f);
	glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), glm::radians(submarineVerticalAngle), glm::vec3(1, 0, 0));
	glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), glm::radians(submarineAngle), glm::vec3(0, 1, 0));
	glm::mat4 rotationMatrixZ = glm::rotate(glm::mat4(1.0f), glm::radians(submarineHorizontalAngle), glm::vec3(0, 0, 1));
	glm::mat4 rotationMatrix = rotationMatrixY * rotationMatrixX * rotationMatrixZ;
	glm::vec4 rotatedPosition = rotationMatrix * glm::vec4(relativePosition, 1.0f);

	// Draw the loaded model
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(submarineX, submarineY, submarineZ) + glm::vec3(rotatedPosition)); // Move to scene centre
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));	// Scale model
	model = glm::rotate(model, glm::radians(submarineAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(submarineVerticalAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(submarineHorizontalAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	shaderModel.SetMat4("model", model);
	objectModel.Draw(shaderModel);
	// ** MODEL **

	return true;
}

bool DrawTreasure(Shader shaderModel, Model objectModel, glm::mat4& view, glm::mat4& projection, float scaleFactor, glm::vec3 pos, float angle, bool collected, int index) {
	// ** MODEL **
	shaderModel.Use();

	view = pCamera->GetViewMatrix();

	shaderModel.SetMat4("view", view);
	shaderModel.SetMat4("projection", projection);
	shaderModel.SetVec3("goldLightPos1", goldLightPos1);
	shaderModel.SetVec3("goldLightPos2", goldLightPos2);
	shaderModel.SetVec3("goldLightPos3", goldLightPos3);
	shaderModel.SetVec3("goldLightPos4", goldLightPos4);
	shaderModel.SetVec3("goldLightPos5", goldLightPos5);
	shaderModel.SetVec3("goldLightPos6", goldLightPos6);
	shaderModel.SetInt("indexTreasure", -1);
	if (!collected)
		shaderModel.SetInt("indexTreasure", index);



	// Draw the loaded model
	glm::mat4 model;
	model = glm::translate(model, pos); // Move to scene centre
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));	// Scale model
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	shaderModel.SetMat4("model", model);
	objectModel.Draw(shaderModel);
	// ** MODEL **

	return true;
}

bool DrawSubmarine(Shader shaderModel, Model objectModel, glm::mat4& view, glm::mat4& projection, float scaleFactor) {
	// ** MODEL **
	shaderModel.Use();

	view = pCamera->GetViewMatrix();

	shaderModel.SetMat4("view", view);
	shaderModel.SetMat4("projection", projection);



	// Draw the loaded model
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(submarineX, submarineY, submarineZ)); // Move to scene centre
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));	// Scale model
	model = glm::rotate(model, glm::radians(submarineAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(submarineVerticalAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(submarineHorizontalAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	shaderModel.SetMat4("model", model);
	objectModel.Draw(shaderModel);
	// ** MODEL **

	return true;
}

bool DrawAlge(Shader shaderModel, Model objectModel, glm::mat4& view, glm::mat4& projection, float heightScaleFactor, float widthScaleFactor, float xPos, float yPos, float zPos, float degrees)
{
	shaderModel.Use();

	view = pCamera->GetViewMatrix();

	shaderModel.SetMat4("view", view);
	shaderModel.SetMat4("projection", projection);
	shaderModel.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);

	// Draw the loaded model
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(xPos, yPos, zPos)); // Move to scene centre
	model = glm::scale(model, glm::vec3(widthScaleFactor, heightScaleFactor, widthScaleFactor)); // Scale model
	model = glm::rotate(model, glm::radians(degrees), glm::vec3(0.0f, 1.0f, 0.0f));
	shaderModel.SetMat4("model", model);
	objectModel.Draw(shaderModel);
	// ** MODEL **

	return true;
}

void DrawVegetation_Cluster1(Shader shaderModel, std::vector<Model> modelVector, glm::mat4& view, glm::mat4& projection, float xPos, float yPos, float zPos)
{
	DrawAlge(shaderModel, modelVector[13], view, projection, 1.0f, 1.0f, -0.7f + xPos, yPos, 0.0f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[13], view, projection, 1.0f, 1.0f, -0.7f + xPos, yPos, 0.0f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[14], view, projection, 1.0f, 1.0f, 0.0f + xPos, yPos, 0.7f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[14], view, projection, 1.0f, 1.0f, 0.0f + xPos, yPos, 0.7f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[16], view, projection, 1.3f, 0.5f, 0.0f + xPos, yPos, 1.5f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[16], view, projection, 1.3f, 0.5f, 0.0f + xPos, yPos, 1.5f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[22], view, projection, 1.3f, 0.7f, 0.0f + xPos, yPos, 0.0f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[22], view, projection, 1.3f, 0.7f, 0.0f + xPos, yPos, 0.0f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[19], view, projection, 1.0f, 1.0f, -0.7f + xPos, yPos, 0.7f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[19], view, projection, 1.0f, 1.0f, -0.7f + xPos, yPos, 0.7f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[16], view, projection, 1.3f, 1.0, -1.4f + xPos, yPos, 1.4f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[16], view, projection, 1.3f, 1.0f, -1.4f + xPos, yPos, 1.4f + zPos, 90.0f);
}

void DrawVegetation_Cluster2(Shader shaderModel, std::vector<Model> modelVector, glm::mat4& view, glm::mat4& projection, float xPos, float yPos, float zPos)
{
	DrawAlge(shaderModel, modelVector[1], view, projection, 1.5f, 0.3f, -0.7f + xPos, yPos, 0.0f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[1], view, projection, 1.5f, 0.3f, -0.7f + xPos, yPos, 0.0f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.7f, 0.5f, +0.7f + xPos, yPos, 0.0f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.7f, 0.5f, +0.7f + xPos, yPos, 0.0f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[4], view, projection, 0.7f, 0.7f, xPos, 0.3f + yPos, zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[4], view, projection, 0.7f, 0.7f, xPos, 0.3f + yPos, zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[4], view, projection, 0.7f, 0.7f, +0.7f + xPos, 0.3f + yPos, -0.7f + +zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[4], view, projection, 0.7f, 0.7f, +0.7f + xPos, 0.3f + yPos, -0.7f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[4], view, projection, 0.7f, 0.7f, -0.7f + xPos, 0.3f + yPos, +0.7f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[4], view, projection, 0.7f, 0.7f, -0.7f + xPos, 0.3f + yPos, +0.7f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[4], view, projection, 0.7f, 0.7f, +0.7f + xPos, 0.3f + yPos, 0.7f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[4], view, projection, 0.7f, 0.7f, +0.7f + xPos, 0.3f + yPos, 0.7f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[4], view, projection, 0.7f, 0.7f, -0.7f + xPos, 0.3f + yPos, -0.7f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[4], view, projection, 0.7f, 0.7f, -0.7f + xPos, 0.3f + yPos, -0.7f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[0], view, projection, 1.7f, 0.5f, -1.7f + xPos, yPos, -0.7f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[0], view, projection, 1.7f, 0.5f, -1.7f + xPos, yPos, -0.7f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[0], view, projection, 1.7f, 0.5f, -0.7f + xPos, yPos, -1.7f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[0], view, projection, 1.7f, 0.5f, -0.7f + xPos, yPos, -1.7f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[0], view, projection, 1.7f, 0.5f, -0.7f + xPos, yPos, -0.7f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[0], view, projection, 1.7f, 0.5f, -0.7f + xPos, yPos, -0.7f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[0], view, projection, 1.7f, 0.5f, -0.7f + xPos, yPos, +1.7f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[0], view, projection, 1.7f, 0.5f, -0.7f + xPos, yPos, +1.7f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.7f, 0.5f, +0.7f + xPos, yPos, -0.7f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.7f, 0.5f, +0.7f + xPos, yPos, -0.7f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.7f, 0.5f, +0.7f + xPos, yPos, +1.7f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.7f, 0.5f, +0.7f + xPos, yPos, +1.7f + zPos, 90.0f);
}

void DrawVegetation_Cluster3(Shader shaderModel, std::vector<Model> modelVector, glm::mat4& view, glm::mat4& projection, float xPos, float yPos, float zPos)
{
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.6f, 0.7f, xPos, yPos, zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[8], view, projection, 1.2f, 0.7f, xPos, yPos, zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.0f, 0.7f, 0.8f + xPos, yPos, zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.3f, 0.7f, 0.8f + xPos, yPos, zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.2f, 0.7f, -0.8f + xPos, yPos, zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.3f, 0.7f, -0.8f + xPos, yPos, zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[0], view, projection, 1.8f, 0.7f, xPos, yPos, 0.8f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.3f, 0.7f, xPos, yPos, 0.8f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[3], view, projection, 1.7f, 0.7f, 0.8f + xPos, yPos, 0.8f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.3f, 0.7f, 0.8f + xPos, yPos, 0.8f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[8], view, projection, 1.4f, 0.7f, -0.8f + xPos, yPos, 0.8f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.3f, 0.7f, -0.8f + xPos, yPos, 0.8f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.2f, 0.7f, xPos, yPos, -0.8f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.3f, 0.7f, xPos, yPos, -0.8f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.0f, 0.7f, 0.8f + xPos, yPos, -0.8f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[0], view, projection, 1.3f, 0.7f, 0.8f + xPos, yPos, -0.8f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[2], view, projection, 1.9f, 0.7f, -0.8f + xPos, yPos, -0.8f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[9], view, projection, 1.2f, 0.7f, -0.8f + xPos, yPos, -0.8f + zPos, 90.0f);
}

void DrawVegetation_Cluster4(Shader shaderModel, std::vector<Model> modelVector, glm::mat4& view, glm::mat4& projection, float xPos, float yPos, float zPos)
{
	DrawAlge(shaderModel, modelVector[11], view, projection, 1.6f, 0.7f, xPos, yPos, zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[21], view, projection, 1.2f, 0.7f, xPos, yPos, zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[21], view, projection, 1.0f, 0.7f, 0.8f + xPos, yPos, zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[21], view, projection, 1.3f, 0.7f, 0.8f + xPos, yPos, zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[12], view, projection, 1.2f, 0.7f, -0.8f + xPos, yPos, zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[21], view, projection, 1.3f, 0.7f, -0.8f + xPos, yPos, zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[21], view, projection, 1.8f, 0.7f, xPos, yPos, 0.8f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[21], view, projection, 1.3f, 0.7f, xPos, yPos, 0.8f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[13], view, projection, 1.7f, 0.7f, 0.8f + xPos, yPos, 0.8f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[21], view, projection, 1.3f, 0.7f, 0.8f + xPos, yPos, 0.8f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[21], view, projection, 1.4f, 0.7f, -0.8f + xPos, yPos, 0.8f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[21], view, projection, 1.3f, 0.7f, -0.8f + xPos, yPos, 0.8f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[21], view, projection, 1.2f, 0.7f, xPos, yPos, -0.8f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[10], view, projection, 1.3f, 0.7f, xPos, yPos, -0.8f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[21], view, projection, 1.0f, 0.7f, 0.8f + xPos, yPos, -0.8f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[21], view, projection, 1.3f, 0.7f, 0.8f + xPos, yPos, -0.8f + zPos, 90.0f);
	DrawAlge(shaderModel, modelVector[15], view, projection, 1.1f, 0.7f, -0.8f + xPos, yPos, -0.8f + zPos, 0.0f);
	DrawAlge(shaderModel, modelVector[21], view, projection, 1.9f, 0.7f, -0.8f + xPos, yPos, -0.8f + zPos, 90.0f);
}

bool DrawWater(Shader shaderModel, Model objectModel, glm::mat4& view, glm::mat4& projection, glm::vec3 sunlightPos, glm::vec3 moonlightPos, float scaleFactor, float yPos) {
	// ** MODEL **
	shaderModel.Use();

	auto currentTime = std::chrono::system_clock::now();
	auto currentTimeSeconds = std::chrono::time_point_cast<std::chrono::seconds>(currentTime);
	auto currentTimeSecondsToInt = currentTimeSeconds.time_since_epoch().count();

	int ambientValue;
	ambientValue = currentTimeSecondsToInt % 86400;
	int segment = ambientValue / 5400;
	float Ka;

	if (segment <= 7) {
		Ka = 0.2 + 0.1 * segment;
	}
	else if (segment <= 15) {
		Ka = 0.8 - 0.1 * (segment - 7);
	}
	else {
		Ka = 0.1;
	}
	view = pCamera->GetViewMatrix();

	shaderModel.SetFloat("Ka", Ka);

	shaderModel.SetMat4("view", view);
	shaderModel.SetMat4("projection", projection);

	shaderModel.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shaderModel.SetVec3("sunlightPos", sunlightPos);
	shaderModel.SetVec3("moonlightPos", moonlightPos);
	shaderModel.SetVec3("viewPos", pCamera->GetPosition());

	// Draw the loaded model
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, yPos, 0.0f)); // Move to scene centre
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));	// Scale model
	//model = glm::rotate(model, degrees, glm::vec3(0.0f, 1.0f, 0.0f));
	shaderModel.SetMat4("model", model);
	objectModel.Draw(shaderModel);
	// ** MODEL **

	return true;
}

bool DrawFish(Shader shaderModel, Model objectModel, glm::mat4& view, glm::mat4& projection, float scaleFactor, float xPos, float yPos, float zPos)
{
	shaderModel.Use();

	view = pCamera->GetViewMatrix();

	shaderModel.SetMat4("view", view);
	shaderModel.SetMat4("projection", projection);
	shaderModel.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);

	// Draw the loaded model
	glm::mat4 model;
	float degreesHardcoded = 90.0f;
	model = glm::translate(model, glm::vec3(xPos, yPos, zPos)); // Move to scene centre
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor)); // Scale model
	model = glm::rotate(model, glm::radians(degreesHardcoded), glm::vec3(-1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(degreesHardcoded), glm::vec3(0.0f, 0.0f, 1.0f));
	shaderModel.SetMat4("model", model);
	objectModel.Draw(shaderModel);
	// ** MODEL **

	return true;
}

void DrawShoal(Shader shaderModel, Model model, glm::mat4& view, glm::mat4& projection, float xPos, float yPos, float zPos)
{
	DrawFish(shaderModel, model, view, projection, 0.03f, 1.5f + xPos, 0.1f + yPos, 0.0f + zPos);
	DrawFish(shaderModel, model, view, projection, 0.03f, 1.4f + xPos, 0.5f + yPos, 0.1f + zPos);
	DrawFish(shaderModel, model, view, projection, 0.03f, 2.0f + xPos, 0.2f + yPos, 0.0f + zPos);
	DrawFish(shaderModel, model, view, projection, 0.03f, 1.7f + xPos, 0.3f + yPos, 0.2f + zPos);
	DrawFish(shaderModel, model, view, projection, 0.03f, 1.9f + xPos, 0.4f + yPos, -0.2f + zPos);
	DrawFish(shaderModel, model, view, projection, 0.03f, 2.0f + xPos, 0.5f + yPos, 0.0f + zPos);
	DrawFish(shaderModel, model, view, projection, 0.03f, 2.2f + xPos, 0.3f + yPos, -0.2f + zPos);
	DrawFish(shaderModel, model, view, projection, 0.03f, 2.1f + xPos, 0.4f + yPos, 0.2f + zPos);
	DrawFish(shaderModel, model, view, projection, 0.03f, 2.2f + xPos, 0.3f + yPos, 0.2f + zPos);
	DrawFish(shaderModel, model, view, projection, 0.03f, 2.3f + xPos, 0.4f + yPos, -0.4f + zPos);
	DrawFish(shaderModel, model, view, projection, 0.03f, 2.4f + xPos, 0.6f + yPos, 0.2f + zPos);
	DrawFish(shaderModel, model, view, projection, 0.03f, 2.5f + xPos, 0.3f + yPos, -0.3f + zPos);
	DrawFish(shaderModel, model, view, projection, 0.03f, 2.3f + xPos, 0.7f + yPos, 0.3f + zPos);
}

bool DrawInterior(Shader shaderModel, Model objectModel, glm::mat4& view, glm::mat4& projection, float scaleFactor)
{
	shaderModel.Use();

	view = pCamera->GetViewMatrix();

	shaderModel.SetMat4("view", view);
	shaderModel.SetMat4("projection", projection);
	shaderModel.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);

	// Draw the loaded model
	glm::vec3 relativePosition(0.0f, 0.16f, -0.4f);

	// Crează o matrice de rotație pentru rotația pe axa Y
	glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), glm::radians(submarineVerticalAngle), glm::vec3(1, 0, 0));
	glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), glm::radians(submarineAngle), glm::vec3(0, 1, 0));
	glm::mat4 rotationMatrix = rotationMatrixY * rotationMatrixX;

	// Aplică rotația asupra poziției relative
	glm::vec4 rotatedPosition = rotationMatrix * glm::vec4(relativePosition, 1.0f);

	// Calculează poziția absolută a camerei adăugând poziția submarinului
	glm::vec3 interiorPosition = glm::vec3(rotatedPosition) + glm::vec3(submarineX, submarineY, submarineZ);
	glm::mat4 model;
	model = glm::translate(model, interiorPosition); // Move to scene centre
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));	// Scale model
	model = glm::rotate(model, glm::radians(submarineAngle - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-submarineVerticalAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	shaderModel.SetMat4("model", model);
	objectModel.Draw(shaderModel);
	// ** MODEL **

	return true;
}

bool DrawShatter(Shader shaderModel, Model objectModel, glm::mat4& view, glm::mat4& projection, float scaleFactor)
{
	shaderModel.Use();

	view = pCamera->GetViewMatrix();

	shaderModel.SetMat4("view", view);
	shaderModel.SetMat4("projection", projection);
	shaderModel.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);

	// Draw the loaded model
	glm::vec3 relativePosition(0.0f, 0.20f, -0.45f);

	// Crează o matrice de rotație pentru rotația pe axa Y
	glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), glm::radians(submarineVerticalAngle), glm::vec3(1, 0, 0));
	glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), glm::radians(submarineAngle), glm::vec3(0, 1, 0));
	glm::mat4 rotationMatrix = rotationMatrixY * rotationMatrixX;

	// Aplică rotația asupra poziției relative
	glm::vec4 rotatedPosition = rotationMatrix * glm::vec4(relativePosition, 1.0f);

	// Calculează poziția absolută a camerei adăugând poziția submarinului
	glm::vec3 interiorPosition = glm::vec3(rotatedPosition) + glm::vec3(submarineX, submarineY, submarineZ);
	glm::mat4 model;
	model = glm::translate(model, interiorPosition); // Move to scene centre
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));	// Scale model
	model = glm::rotate(model, glm::radians(submarineAngle - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-submarineVerticalAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	shaderModel.SetMat4("model", model);
	objectModel.Draw(shaderModel);
	// ** MODEL **

	return true;
}

bool DrawLightSource(Shader shaderModel, Model objectModel, glm::mat4& view, glm::mat4& projection, glm::vec3 lightPos, float scaleFactor) {
	// ** MODEL **
	shaderModel.Use();

	auto currentTime = std::chrono::system_clock::now();
	auto currentTimeSeconds = std::chrono::time_point_cast<std::chrono::seconds>(currentTime);
	auto currentTimeSecondsToInt = currentTimeSeconds.time_since_epoch().count();
	view = pCamera->GetViewMatrix();

	shaderModel.SetMat4("view", view);
	shaderModel.SetMat4("projection", projection);

	shaderModel.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shaderModel.SetVec3("viewPos", pCamera->GetPosition());

	// Draw the loaded model
	glm::mat4 model;
	model = glm::translate(model, lightPos); // Move to scene centre
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));	// Scale model
	//model = glm::rotate(model, degrees, glm::vec3(0.0f, 1.0f, 0.0f));
	shaderModel.SetMat4("model", model);
	objectModel.Draw(shaderModel);
	// ** MODEL **

	return true;
}

bool BuildDepthMapVBO(unsigned int& depthMap, unsigned int& depthMapFBO) {

	// configure depth map FBO
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

bool RenderSceneWithLight(Shader shadowMappingDepthShader, Shader shadowMappingShader, unsigned int depthMap, unsigned int depthMapFBO, glm::vec3 lightPos, glm::mat4& view, glm::mat4& projection) {
	// render
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1. render depth of scene to texture (from light's perspective)
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 7.5f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;

	// render scene from light's point of view
	shadowMappingDepthShader.Use();
	shadowMappingDepthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 2. render scene as normal using the generated depth/shadow map 
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shadowMappingShader.Use();

	// set light uniforms
	shadowMappingShader.SetMat4("projection", projection);
	shadowMappingShader.SetMat4("view", view);
	shadowMappingShader.SetVec3("viewPos", pCamera->GetPosition());

	shadowMappingShader.SetVec3("lightPos", lightPos);
	shadowMappingShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDisable(GL_CULL_FACE);

	return true;
}

// lighting info
glm::vec3 sunlightPos(-2.0f, 4.0f, -1.0f);
glm::vec3 moonlightPos(-2.0f, 4.0f, -1.0f);

int cameraType = 2;
void processCameraType(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		cameraType = 0;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		cameraType = 1;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		cameraType = 2;
}

bool InitializeWindow(GLFWwindow*& window) {
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Yellow Submarine", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewInit();

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	return true;
}

void calculateFaceNormals(std::vector<Face>& faces) {
	for (auto& face : faces) {
		glm::vec3 edge1 = face.vertices[1] - face.vertices[0];
		glm::vec3 edge2 = face.vertices[2] - face.vertices[0];
		face.normal = glm::normalize(glm::cross(edge1, edge2));
	}
}

int main(int argc, char** argv) {

	auto t_start = std::chrono::high_resolution_clock::now();

	GLFWwindow* window;
	InitializeWindow(window);

	auto currentTime = std::chrono::system_clock::now();
	auto currentTimeSeconds = std::chrono::time_point_cast<std::chrono::seconds>(currentTime);
	auto currentTimeSecondsToInt = currentTimeSeconds.time_since_epoch().count();

	// Build and compile shaders
	std::string strFullExeFileName = argv[0];
	InitializePaths(strFullExeFileName);

	Shader shadowMappingShader(pathToShadowMappingShaders + "ShadowMapping.vs", pathToShadowMappingShaders + "ShadowMapping.fs");
	Shader shadowMappingDepthShader(pathToShadowMappingShaders + "ShadowMappingDepth.vs", pathToShadowMappingShaders + "ShadowMappingDepth.fs");

	std::string pathToObjectShaders(pathToShaders + "Objects\\");
	Shader shaderModel(pathToObjectShaders + "modelLoading.vs", pathToObjectShaders + "modelLoading.frag");
	Shader shaderAlge(pathToObjectShaders + "algeShader.vs", pathToObjectShaders + "algeShader.frag");
	Shader shaderInterior(pathToObjectShaders + "interiorShader.vs", pathToObjectShaders + "interiorShader.frag");
	Shader shaderShatter(pathToObjectShaders + "shatterShader.vs", pathToObjectShaders + "shatterShader.frag");
	Shader shaderLighting(pathToObjectShaders + "lightingShader.vs", pathToObjectShaders + "lightingShader.frag");

	// Load models
	std::string pathSub = pathToDetachedSubmarine + "sub_obj.obj";
	std::string pathProp = pathToDetachedSubmarine + "prop_obj.obj";
	std::string pathFlap = pathToDetachedSubmarine + "flap_obj.obj";
	std::string pathHublou = pathToHublou + "hublou_obj.obj";
	std::string pathTreasure = pathToTreasure + "treasure.obj";
	std::string pathEmptyTreasure = pathToTreasure + "cufar.obj";
	std::string pathTerrain = pathToTerrain + "terrain.obj";
	std::string pathWater = pathToWater + "water.obj";
	std::string pathInterior = pathToInterior + "interior.obj";
	std::string pathShatter = pathToShatter + "shatter.obj";
	std::string pathSun = pathToSun + "13913_Sun_v2_l3.obj";
	std::string pathMoon = pathToMoon + "moon.obj";

	std::vector<std::string> pathAlge;
	for (int index = 0; index < 23; index++)
		pathAlge.push_back({ pathToAlge + "alga" + std::to_string(index) + ".obj" });

	std::string pathFish = pathToFish + "fish_obj1.obj";

	std::string pathHitbox = pathToHitbox + "hitbox.obj";
	std::string pathWall = pathToHitbox + "hitboxWall.obj";
	std::string pathTreasureHitbox = pathToHitbox + "hitboxTreasure.obj";


	const char* submarine = pathSub.c_str();
	Model submarineModel((GLchar*)submarine);

	const char* propeller = pathProp.c_str();
	Model propellerModel((GLchar*)propeller);

	const char* flap = pathFlap.c_str();
	Model flapModel((GLchar*)flap);

	const char* hublou = pathHublou.c_str();
	Model hublouModel((GLchar*)hublou);

	const char* terrain = pathTerrain.c_str();
	Model terrainModel((GLchar*)terrain);

	const char* treasure = pathTreasure.c_str();
	Model treasureModel((GLchar*)treasure);

	const char* emptyTreasure = pathEmptyTreasure.c_str();
	Model emptyTreasureModel((GLchar*)emptyTreasure);

	const char* interior = pathInterior.c_str();
	Model interiorModel((GLchar*)interior);

	const char* sun = pathSun.c_str();
	Model sunModel((GLchar*)sun);

	const char* moon = pathMoon.c_str();
	Model moonModel((GLchar*)moon);

	const char* shatter = pathShatter.c_str();
	Model shatterModel((GLchar*)shatter);

	std::vector<const char*> alge;
	for (int index = 0; index < 23; index++)
		alge.push_back(pathAlge[index].c_str());

	std::vector<Model> algeModel;
	for (int index = 0; index < 23; index++)
		algeModel.push_back(Model((GLchar*)alge[index]));

	const char* fish = pathFish.c_str();
	Model fishModel((GLchar*)fish);

	const char* hitbox = pathHitbox.c_str();
	Model hitboxModel((GLchar*)hitbox);
	std::vector<Vertex> submarineVertex = hitboxModel.GetMeshes()[0].vertices;
	for (Vertex& vertex : submarineVertex)
		submarineInitialHitbox.push_back(vertex.Position);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(pathWall.c_str(),
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality);

	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		std::cerr << "Error: " << importer.GetErrorString() << std::endl;
		return -1;
	}

	std::vector<Face> faces;

	// Parcurgem toate mesh-urile
	for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];

		// Extragem toate fețele (triunghiurile) din mesh
		for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& aiFace = mesh->mFaces[faceIndex];

			if (aiFace.mNumIndices == 3) { // Ne asigurăm că avem un triunghi
				Face face;
				for (unsigned int i = 0; i < 3; ++i) {
					aiVector3D aiVertex = mesh->mVertices[aiFace.mIndices[i]];
					face.vertices[i] = glm::vec3(aiVertex.x * 0.2, aiVertex.y * 0.2, aiVertex.z * 0.2);
				}
				faces.push_back(face);
			}
		}
	}

	calculateFaceNormals(faces);

	scene = importer.ReadFile(pathTreasureHitbox.c_str(),
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality);

	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		std::cerr << "Error: " << importer.GetErrorString() << std::endl;
		return -1;
	}

	std::vector<Treasure> treasureHitboxes;

	std::vector<glm::vec3> treasurePositions =
	{
		glm::vec3(-0.475f, 0.42f, -34.0f),
		glm::vec3(-27.5f, -2.95f, -16.5f),
		glm::vec3(-3.75f, -3.04f, 4.45f),
		glm::vec3(-45.0f, 0.47f, 15.5f),
		glm::vec3(-22.0f, -3.0f, 2.0f),
		glm::vec3(10.6f, 0.2f, 24.5f),
	};

	std::vector<float> treasureRotations = { 180.0f, 0.0f, 0.0f, -20.0f, -90.0f, 225.0f };

	for (int index = 0; index < treasurePositions.size(); index++)
	{
		Treasure treasure;
		for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
			aiMesh* mesh = scene->mMeshes[meshIndex];

			// Extragem toate fețele (triunghiurile) din mesh
			for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
				aiFace& aiFace = mesh->mFaces[faceIndex];

				if (aiFace.mNumIndices == 3) { // Ne asigurăm că avem un triunghi
					Face face;
					for (unsigned int i = 0; i < 3; ++i) {
						aiVector3D aiVertex = mesh->mVertices[aiFace.mIndices[i]];
						face.vertices[i] = glm::vec3(aiVertex.x + treasurePositions[index].x, aiVertex.y + treasurePositions[index].y, aiVertex.z + treasurePositions[index].z);
					}
					treasure.hitbox.push_back(face);
				}
			}
		}
		calculateFaceNormals(treasure.hitbox);
		treasureHitboxes.push_back(treasure);
	}

	const char* wall = pathWall.c_str();
	Model wallModel((GLchar*)wall);
	std::vector<Vertex> wallVertex = wallModel.GetMeshes()[0].vertices;
	for (Vertex& vertex : wallVertex)
		terrainInitialHitbox.push_back(vertex.Position);

	unsigned int depthMap;
	unsigned int depthMapFBO;
	BuildDepthMapVBO(depthMap, depthMapFBO);

	// shader configuration
	shadowMappingShader.Use();
	shadowMappingShader.SetInt("diffuseTexture", 0);
	shadowMappingShader.SetInt("shadowMap", 1);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 projection = glm::perspective(pCamera->GetZoom(), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	// ** SKYBOX **
	Shader shaderCubeMap(pathToSkyBoxShaders + "cubemaps.vs", pathToSkyBoxShaders + "cubemaps.fs");
	Shader shaderSkybox(pathToSkyBoxShaders + "skybox.vs", pathToSkyBoxShaders + "skybox.fs");
	buildSkybox(shaderCubeMap, shaderSkybox, pathToTextures);
	// ** SKYBOX **

	// ** WATER **
	Shader shaderWater(pathToWaterShaders + "water.vs", pathToWaterShaders + "water.fs");

	const char* water = pathWater.c_str();
	Model waterModel((GLchar*)water);
	// ** WATER **

	// ** RENDER LOOP **
	while (!glfwWindowShouldClose(window)) {

		// per-frame time logic
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		std::string pathSound = pathToSounds + "music.mp3";
		if (!(engine->isCurrentlyPlaying(pathSound.c_str())))
			engine->play2D(pathSound.c_str(), false);
		//glm::radians(360.f)=6.28319 (am folosit constanta ca nu mergea daca foloseam glm::radians
		sunlightPos.x = 60 * sin(((6.28319 / 86400) * (currentTimeSecondsToInt + 45300))) - 15;
		sunlightPos.y = 60 * cos(((6.28319 / 86400) * (currentTimeSecondsToInt + 45300)));
		moonlightPos.x = 60 * sin(((6.28319 / 86400) * (currentTimeSecondsToInt + 2200))) - 15;
		moonlightPos.y = 60 * cos(((6.28319 / 86400) * (currentTimeSecondsToInt + 2200)));
		//std::cout << lightPos.x << " " << lightPos.y << " " << currentTimeSecondsToInt << " " << 6.28319 * currentTimeSecondsToInt << " " << "\n";

	// input
		processInput(window);
		processSubmarineMovement(window, faces, treasureHitboxes);
		processCameraType(window);

		if (cameraType == 0)
		{
			// Poziția relativă inițială a camerei față de submarin
			glm::vec3 relativePosition(0.0f, 0.25f, -0.25f);

			// Crează o matrice de rotație pentru rotația pe axa Y
			glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), glm::radians(submarineVerticalAngle), glm::vec3(1, 0, 0));
			glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), glm::radians(submarineAngle), glm::vec3(0, 1, 0));
			glm::mat4 rotationMatrix = rotationMatrixY * rotationMatrixX;

			// Aplică rotația asupra poziției relative
			glm::vec4 rotatedPosition = rotationMatrix * glm::vec4(relativePosition, 1.0f);

			// Calculează poziția absolută a camerei adăugând poziția submarinului
			glm::vec3 cameraPosition = glm::vec3(rotatedPosition) + glm::vec3(submarineX, submarineY, submarineZ);

			int width, height;
			glfwGetWindowSize(window, &width, &height);
			pCamera->Set(width, height, cameraPosition, -submarineAngle - 90.0f, submarineVerticalAngle);
		}

		if (cameraType == 1)
		{
			// Poziția relativă inițială a camerei față de submarin
			glm::vec3 relativePosition(0.0f, 0.25f, 3.00f);

			// Crează o matrice de rotație pentru rotația pe axa Y
			glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), glm::radians(submarineVerticalAngle), glm::vec3(1, 0, 0));
			glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), glm::radians(submarineAngle), glm::vec3(0, 1, 0));
			glm::mat4 rotationMatrix = rotationMatrixY * rotationMatrixX;

			// Aplică rotația asupra poziției relative
			glm::vec4 rotatedPosition = rotationMatrix * glm::vec4(relativePosition, 1.0f);

			// Calculează poziția absolută a camerei adăugând poziția submarinului
			glm::vec3 cameraPosition = glm::vec3(rotatedPosition) + glm::vec3(submarineX, submarineY, submarineZ);

			int width, height;
			glfwGetWindowSize(window, &width, &height);
			pCamera->Set(width, height, cameraPosition, -submarineAngle - 90.0f, submarineVerticalAngle);
		}

		glm::mat4 view = pCamera->GetViewMatrix();
		RenderSceneWithLight(shadowMappingDepthShader, shadowMappingShader, depthMap, depthMapFBO, sunlightPos, view, projection);

		DrawSkybox(shaderSkybox, view, projection);

		// ** MODEL **
		DrawSubmarine(shaderModel, submarineModel, view, projection, 0.002f);
		DrawPropeller(shaderModel, propellerModel, view, projection, 0.002f, 0.063f);
		DrawPropeller(shaderModel, propellerModel, view, projection, 0.002f, 0.345f);
		DrawFlaps(shaderModel, flapModel, view, projection, 0.002f, -0.15f, 0.0f);
		DrawFlaps(shaderModel, flapModel, view, projection, 0.002f, 0.15f, 180.0f);
		DrawHublou(shaderModel, hublouModel, view, projection, 0.002f);

		for (int index = 0; index < treasurePositions.size(); index++)
		{
			//std::cout << index << " " << treasureHitboxes[index].collected << "\n";
			if (!treasureHitboxes[index].collected)
				DrawTreasure(shaderModel, treasureModel, view, projection, 0.1f, treasurePositions[index], treasureRotations[index], treasureHitboxes[index].collected, index);
			else
				DrawTreasure(shaderModel, emptyTreasureModel, view, projection, 0.1f, treasurePositions[index], treasureRotations[index], treasureHitboxes[index].collected, index);
		}


		auto t_now = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

		//DrawAndRotateObject(shaderModel, propellerModel, view, projection, 0.5f, time, 2.0);
		//DrawAndRotateObject(shaderModel, submarineModel, view, projection, 0.5f, time, 2.0);

		DrawObject(shaderModel, terrainModel, view, projection, sunlightPos, moonlightPos, 0.2f);
		DrawLightSource(shaderLighting, moonModel, view, projection, moonlightPos, 0.005f);
		DrawLightSource(shaderLighting, sunModel, view, projection, sunlightPos, 0.005f);
		if (cameraType == 0)
		{
			DrawInterior(shaderInterior, interiorModel, view, projection, 0.01f);
			if (broken)
				DrawShatter(shaderShatter, shatterModel, view, projection, 0.01f);
		}

		//DrawObject(shaderModel, wallModel, view, projection, sunlightPos, moonlightPos, 0.2f);
		// ** MODEL **

		DrawVegetation_Cluster2(shaderAlge, algeModel, view, projection, -2.5f, -2.5f, -6.5f);
		DrawVegetation_Cluster2(shaderAlge, algeModel, view, projection, -3.5f, -3.5f, -6.0f);
		DrawVegetation_Cluster2(shaderAlge, algeModel, view, projection, -1.5f, -3.5f, -7.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, 2.5f, -3.5f, -7.0f);
		DrawVegetation_Cluster1(shaderAlge, algeModel, view, projection, 10.0f, -2.5f, 0.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, 7.0f, -3.5f, 9.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, 9.0f, -2.0f, 8.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, 8.5f, -3.5f, 5.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, 8.5f, -3.5f, 11.0f);
		DrawVegetation_Cluster1(shaderAlge, algeModel, view, projection, 7.0f, -3.5f, 6.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, -20.0f, -3.5f, 8.0f);
		DrawVegetation_Cluster2(shaderAlge, algeModel, view, projection, -22.0f, -3.5f, 3.0f);
		DrawVegetation_Cluster2(shaderAlge, algeModel, view, projection, -22.0f, -3.5f, 15.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, -30.0f, -3.5f, 2.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, -25.0f, -3.5f, 18.0f);
		DrawVegetation_Cluster2(shaderAlge, algeModel, view, projection, -32.0f, -3.0f, 22.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, -34.0f, -3.5f, 10.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, -16.0f, -3.5f, -16.0f);
		DrawVegetation_Cluster1(shaderAlge, algeModel, view, projection, -26.0f, -3.5f, -30.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, -23.0f, -3.5f, -8.0f);
		DrawVegetation_Cluster2(shaderAlge, algeModel, view, projection, -24.5f, -3.5f, -11.0f);
		DrawVegetation_Cluster1(shaderAlge, algeModel, view, projection, -25.0f, -3.5f, -13.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, -30.0f, -2.5f, -20.6f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, -27.0f, -3.5f, -15.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, -30.0f, -2.5f, 17.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, -30.0f, -2.5f, -35.0f);
		DrawVegetation_Cluster2(shaderAlge, algeModel, view, projection, -36.0f, -3.0f, -30.0f);
		DrawVegetation_Cluster2(shaderAlge, algeModel, view, projection, -34.0f, -2.0f, -33.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, -33.0f, -3.0f, -28.0f);
		DrawVegetation_Cluster2(shaderAlge, algeModel, view, projection, -35.0f, -2.5f, -30.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, -28.0f, -2.5f, -37.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, -16.0f, -2.5f, -35.0f);
		DrawVegetation_Cluster1(shaderAlge, algeModel, view, projection, -18.0f, -2.5f, -34.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, -14.0f, -2.5f, -33.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, -12.0f, -2.5f, -35.0f);
		DrawVegetation_Cluster1(shaderAlge, algeModel, view, projection, -8.0f, -3.5f, -18.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, -3.0f, -3.5f, -18.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, -1.0f, -3.5f, -17.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, 0.0f, -3.0f, -15.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, -24.0f, -3.0f, 3.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, -20.0f, -3.0f, 5.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, -18.0f, -3.0f, 7.0f);
		DrawVegetation_Cluster2(shaderAlge, algeModel, view, projection, -38.0f, -3.0f, 13.0f);
		DrawVegetation_Cluster2(shaderAlge, algeModel, view, projection, -42.0f, -3.0f, 17.0f);
		DrawVegetation_Cluster1(shaderAlge, algeModel, view, projection, 10.0f, -1.5f, -15.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, 12.0f, -1.5f, -17.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, 12.0f, -1.5f, -8.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, -4.0f, -3.5f, 4.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, -3.0f, -3.5f, 4.5f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, -5.0f, -3.5f, 5.5f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, -7.0f, -3.5f, 7.5f);
		DrawVegetation_Cluster1(shaderAlge, algeModel, view, projection, -5.0f, -3.5f, 2.0f);
		DrawVegetation_Cluster1(shaderAlge, algeModel, view, projection, 5.0f, -2.0f, 20.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, 4.0f, -2.0f, 22.0f);
		DrawVegetation_Cluster1(shaderAlge, algeModel, view, projection, 6.0f, -2.0f, 23.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, 6.0f, -2.0f, 19.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, -1.0f, -3.0f, 22.0f);
		DrawVegetation_Cluster1(shaderAlge, algeModel, view, projection, -2.0f, -3.0f, 23.0f);
		DrawVegetation_Cluster2(shaderAlge, algeModel, view, projection, -4.0f, -3.0f, 19.0f);
		DrawVegetation_Cluster2(shaderAlge, algeModel, view, projection, 0.0f, -2.5f, 40.0f);
		DrawVegetation_Cluster4(shaderAlge, algeModel, view, projection, -7.0f, -2.5f, 40.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, 5.0f, -2.5f, 30.0f);
		DrawVegetation_Cluster3(shaderAlge, algeModel, view, projection, -3.5f, -2.5f, 33.0f);

		//DrawShoal(shaderModel, fishModel, view, projection, 0.0f, -2.0f, 0.0f);
		DrawShoal(shaderModel, fishModel, view, projection, 0.0f, -1.0f, -15.0f);
		DrawShoal(shaderModel, fishModel, view, projection, -8.0f, 0.0f, 0.0f);
		DrawShoal(shaderModel, fishModel, view, projection, -15.0f, 0.0f, -15.0f);
		//DrawShoal(shaderModel, fishModel, view, projection, -34.0f, 0.0f, -20.0f);
		DrawShoal(shaderModel, fishModel, view, projection, -30.0f, 0.0f, -30.0f);
		//DrawShoal(shaderModel, fishModel, view, projection, -25.0f, 0.0f, -17.0f);
		DrawShoal(shaderModel, fishModel, view, projection, 2.0f, 0.0f, 15.0f);
		//DrawShoal(shaderModel, fishModel, view, projection, -2.0f, 0.0f, 25.0f);
		DrawShoal(shaderModel, fishModel, view, projection, 0.0f, 0.0f, 35.0f);
		//DrawShoal(shaderModel, fishModel, view, projection, -1.0f, 0.0f, 36.0f);
		DrawShoal(shaderModel, fishModel, view, projection, -22.0f, 0.0f, 16.0f);
		DrawShoal(shaderModel, fishModel, view, projection, -32.0f, -0.5f, 12.0f);
		//DrawShoal(shaderModel, fishModel, view, projection, -17.0f, -1.0f, 10.0f);
		DrawShoal(shaderModel, fishModel, view, projection, -27.0f, -1.0f, 13.0f);

		// Set the opacity value
		float opacity = 0.65f;
		shaderWater.Use();
		shaderWater.SetFloat("opacity", opacity);
		DrawWater(shaderWater, waterModel, view, projection, sunlightPos, moonlightPos, 0.2f, 0.0f);
		opacity = 0.45f;
		shaderWater.Use();
		shaderWater.SetFloat("opacity", opacity);
		DrawWater(shaderWater, waterModel, view, projection, sunlightPos, moonlightPos, 0.2f, 0.002f);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// ** RENDER LOOP **

	// optional: de-allocate all resources once they've outlived their purpose:
	delete pCamera;

	glfwTerminate();
	return 0;
}