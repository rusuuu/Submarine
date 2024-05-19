#pragma once

#include <string>

std::string pathToRootFolder;
std::string pathToExe;

std::string pathToExternalTextures;

std::string pathToTextures;
std::string pathToShaders;
std::string pathToObjects;
std::string pathToSounds;

std::string pathToSubmarine;
std::string pathToDetachedSubmarine;
std::string pathToHublou;

std::string pathToTerrain;
std::string pathToWater;
std::string pathToAlge;
std::string pathToFish;
std::string pathToInterior;
std::string pathToShatter;
std::string pathToSun;
std::string pathToMoon;
std::string pathToHitbox;

std::string pathToSkyBoxTextures;
std::string pathToSkyBoxShaders;
std::string pathToShadowMappingShaders;
std::string pathToWaterShaders;

bool InitializePaths(std::string strFullExeFileName) {	//Initializes all the paths to the directories of the project

	pathToRootFolder = strFullExeFileName;
	for (size_t i = 0; i < 3; i++) {
		size_t last_slash_index = pathToRootFolder.rfind('\\');
		pathToRootFolder = pathToRootFolder.substr(0, last_slash_index);
	}

	pathToExe = strFullExeFileName;
	for (size_t i = 0; i < 2; i++) {
		size_t last_slash_index = pathToExe.rfind('\\');
		pathToExe = pathToExe.substr(0, last_slash_index);
	}
	pathToExe = pathToExe + "\\YellowSubmarine";

	pathToExternalTextures = pathToRootFolder + "\\_external\\Textures\\";

	pathToTextures = pathToExe + "\\Textures\\";
	pathToShaders = pathToExe + "\\Shaders\\";
	pathToObjects = pathToExe + "\\Objects\\";
	pathToSounds = pathToExe + "\\Sounds\\";

	pathToSubmarine = pathToExe + "\\Objects\\Submarine\\";
	pathToDetachedSubmarine = pathToExe + "\\Objects\\SubmarinePropellor\\";
	pathToHublou = pathToExe + "\\Objects\\Hublou\\";
	
	pathToTerrain = pathToExe + "\\Objects\\Terrain\\";
	pathToWater = pathToExe + "\\Objects\\Water\\";
	pathToAlge = pathToExe + "\\Objects\\Alge\\";
	pathToFish = pathToExe + "\\Objects\\Fish\\";
	pathToInterior = pathToExe + "\\Objects\\Interior\\";
	pathToShatter = pathToExe + "\\Objects\\Shatter\\";
	pathToSun = pathToExe + "\\Objects\\Sun\\";
	pathToMoon = pathToExe + "\\Objects\\Moon\\";
	pathToHitbox = pathToExe + "\\Objects\\Hitboxes\\";

	pathToSkyBoxTextures = pathToTextures + "Skybox\\";
	pathToSkyBoxShaders = pathToShaders + "Skybox\\";
	pathToShadowMappingShaders = pathToShaders + "Shadow\\";
	pathToWaterShaders = pathToShaders + "Water\\";

	return true;
}
