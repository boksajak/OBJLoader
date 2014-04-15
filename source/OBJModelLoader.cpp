#include "OBJModelLoader.h"


bool OBJModelLoader::LoadOBJModel(char* baseDirectory, const char* fileName) {
	tempBaseDirectory = baseDirectory;

	char filePath[2* BASE_DIR_MAX_LENGTH];

	strcpy_s(filePath, baseDirectory);
	strcat_s(filePath, fileName);

	trianglesCount = 0;

	objLoader->loadFromFile(filePath, this);

	std::cout << "Loaded " << trianglesCount << " triangles." << std::endl;
	return true;
};