// YellowSubmarine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdlib.h> // necesare pentru citirea shaderStencilTesting-elor

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
	float Ka;
	if (ambientValue >= 0 && ambientValue < 5400)
		Ka = 0.3;
	if (ambientValue >= 5400 && ambientValue < 10800)
		Ka = 0.4;
	if (ambientValue >= 10800 && ambientValue < 16200)
		Ka = 0.5;
	if (ambientValue >= 16200 && ambientValue < 21600)
		Ka = 0.6;
	if (ambientValue >= 21600 && ambientValue < 27000)
		Ka = 0.7;
	if (ambientValue >= 27000 && ambientValue < 32400)
		Ka = 0.8;
	if (ambientValue >= 32400 && ambientValue < 37800)
		Ka = 0.8;
	if (ambientValue >= 37800 && ambientValue < 43200)
		Ka = 0.7;
	if (ambientValue >= 43200 && ambientValue < 48600)
		Ka = 0.6;
	if (ambientValue >= 48600 && ambientValue < 54000)
		Ka = 0.5;
	if (ambientValue >= 54000 && ambientValue < 59400)
		Ka = 0.4;
	if (ambientValue >= 59400 && ambientValue < 64800)
		Ka = 0.3;
	if (ambientValue >= 64800 && ambientValue < 70200)
		Ka = 0.2;
	if (ambientValue >= 70200 && ambientValue < 75600)
		Ka = 0.1;
	if (ambientValue >= 75600 && ambientValue < 81000)
		Ka = 0.1;
	if (ambientValue >= 81000 && ambientValue < 86400)
		Ka = 0.2;

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
	float Ka;
	if (ambientValue >= 0 && ambientValue < 5400)
		Ka = 0.3;
	if (ambientValue >= 5400 && ambientValue < 10800)
		Ka = 0.4;
	if (ambientValue >= 10800 && ambientValue < 16200)
		Ka = 0.5;
	if (ambientValue >= 16200 && ambientValue < 21600)
		Ka = 0.6;
	if (ambientValue >= 21600 && ambientValue < 27000)
		Ka = 0.7;
	if (ambientValue >= 27000 && ambientValue < 32400)
		Ka = 0.8;
	if (ambientValue >= 32400 && ambientValue < 37800)
		Ka = 0.8;
	if (ambientValue >= 37800 && ambientValue < 43200)
		Ka = 0.7;
	if (ambientValue >= 43200 && ambientValue < 48600)
		Ka = 0.6;
	if (ambientValue >= 48600 && ambientValue < 54000)
		Ka = 0.5;
	if (ambientValue >= 54000 && ambientValue < 59400)
		Ka = 0.4;
	if (ambientValue >= 59400 && ambientValue < 64800)
		Ka = 0.3;
	if (ambientValue >= 64800 && ambientValue < 70200)
		Ka = 0.2;
	if (ambientValue >= 70200 && ambientValue < 75600)
		Ka = 0.1;
	if (ambientValue >= 75600 && ambientValue < 81000)
		Ka = 0.1;
	if (ambientValue >= 81000 && ambientValue < 86400)
		Ka = 0.2;
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



	// Draw the loaded model
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(submarineX, submarineY + yPos, submarineZ)); // Move to scene centre
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));	// Scale model
	model = glm::rotate(model, glm::radians(submarineAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(propellerAngle), glm::vec3(0.0f, 0.0f, 1.0f));
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

	// Load models
	std::string pathSub = pathToDetachedSubmarine + "sub_obj.obj";
	std::string pathProp = pathToDetachedSubmarine + "prop_obj.obj";
	std::string pathTerrain = pathToTerrain + "terrain.obj";
	std::string pathWater = pathToWater + "water.obj";

	std::vector<std::string> pathAlge;
	for (int index = 0; index < 23; index++)
		pathAlge.push_back({ pathToAlge + "alga" + std::to_string(index) + ".obj" });

	std::string pathHitbox = pathToHitbox + "hitbox.obj";
	std::string pathWall = pathToHitbox + "hitboxWall.obj";

	const char* submarine = pathSub.c_str();
	Model submarineModel((GLchar*)submarine);

	const char* propeller = pathProp.c_str();
	Model propellerModel((GLchar*)propeller);

	const char* terrain = pathTerrain.c_str();
	Model terrainModel((GLchar*)terrain);

	std::vector<const char*> alge;
	for (int index = 0; index < 23; index++)
		alge.push_back(pathAlge[index].c_str());

	std::vector<Model> algeModel;
	for (int index = 0; index < 23; index++)
		algeModel.push_back(Model((GLchar*)alge[index]));

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
					face.vertices[i] = glm::vec3(aiVertex.x*0.2, aiVertex.y*0.2, aiVertex.z*0.2);
				}
				faces.push_back(face);
			}
		}
	}

	calculateFaceNormals(faces);



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

		//glm::radians(360.f)=6.28319 (am folosit constanta ca nu mergea daca foloseam glm::radians
		sunlightPos.x = 60 * sin(((6.28319 / 86400) * (currentTimeSecondsToInt + 52500))) - 15;
		sunlightPos.y = 60 * cos(((6.28319 / 86400) * (currentTimeSecondsToInt + 52500)));
		moonlightPos.x = 60 * sin(((6.28319 / 86400) * (currentTimeSecondsToInt + 9300))) - 15;
		moonlightPos.y = 60 * cos(((6.28319 / 86400) * (currentTimeSecondsToInt + 9300)));
		//std::cout << lightPos.x << " " << lightPos.y << " " << currentTimeSecondsToInt << " " << 6.28319 * currentTimeSecondsToInt << " " << "\n";

	// input
		processInput(window);
		processSubmarineMovement(window, faces);

		glm::mat4 view = pCamera->GetViewMatrix();
		RenderSceneWithLight(shadowMappingDepthShader, shadowMappingShader, depthMap, depthMapFBO, sunlightPos, view, projection);

		DrawSkybox(shaderSkybox, view, projection);

		// ** MODEL **
		DrawSubmarine(shaderModel, submarineModel, view, projection, 0.002f);
		DrawPropeller(shaderModel, propellerModel, view, projection, 0.002f, 0.063f);
		DrawPropeller(shaderModel, propellerModel, view, projection, 0.002f, 0.35f);

		auto t_now = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

		//DrawAndRotateObject(shaderModel, propellerModel, view, projection, 0.5f, time, 2.0);
		//DrawAndRotateObject(shaderModel, submarineModel, view, projection, 0.5f, time, 2.0);

		DrawObject(shaderModel, terrainModel, view, projection, sunlightPos, moonlightPos, 0.2f);
		DrawObject(shaderModel, waterModel, view, projection, sunlightPos, moonlightPos, 0.2f);
		DrawObject(shaderModel, wallModel, view, projection, sunlightPos, moonlightPos, 0.2f);
		// ** MODEL **

		for (int index = 0; index < 23; index++)
		{
			DrawAlge(shaderAlge, algeModel[index], view, projection, 1.0f, 0.3, 0.0f, -3.5f, index, 0.0f);
			DrawAlge(shaderAlge, algeModel[index], view, projection, 1.0f, 0.3f, 0.0f, -3.5f, index, 90.0f);
		}

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