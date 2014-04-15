#pragma once

#include "OBJLoader.h"
#include <vector>
#include <assert.h>

#define BASE_DIR_MAX_LENGTH 256

struct Vertex {
	OBJ::OBJVec3 normal;
	OBJ::OBJVec3 position;
	OBJ::OBJVec3 texCoords;
};

struct Triangle {
	Vertex A, B, C;
};

class OBJModelLoader : OBJ::IOBJLoaderListener {
public:
	
	OBJModelLoader() : objLoader(NULL) { }

	OBJ::OBJLoader* objLoader;

	void Initialize() {
		assert(!objLoader);
		objLoader = new OBJ::OBJLoader();
	}

	void Deinitialize() {
		if (objLoader) {
			delete objLoader;
			objLoader = NULL;
		}
	}
	
	void newTriangle(const OBJ::OBJTriangle &triangle) {
		Triangle t;

		t.A.position = triangle.position[0];
		t.B.position = triangle.position[1];
		t.C.position = triangle.position[2];

		t.A.normal = triangle.normal[0];
		t.B.normal = triangle.normal[1];
		t.C.normal = triangle.normal[2];

		t.A.texCoords = triangle.texCoords[0];
		t.B.texCoords = triangle.texCoords[1];
		t.C.texCoords = triangle.texCoords[2];

		tempVertices.push_back(t.A);
		tempVertices.push_back(t.B);
		tempVertices.push_back(t.C);

		trianglesCount++;
	}

	void newCamera(const OBJ::OBJCamera &camera) { }

	void newPointLight(OBJ::OBJVec3 position) {	}

	void useMaterial(const char* material) { }

	void newMaterial(const OBJ::OBJMaterial &material) { }

	void Error(const OBJ::OBJError error, const char* data) { 

		switch (error) {
		case OBJ::OBJError::UNKNOWN_COMMAND:
			std::cout << "Unknown command (" << data << ") while processing OBJ File was encountered!" << std::endl;
			break;
		default:
			std::cout << "Unknown error while processing OBJ File was encountered! (" << data << ")" << std::endl;
		}

	}

	bool LoadOBJModel(char* baseDirectory, const char* fileName);

private:
	
	std::vector<Vertex> tempVertices;
	char* tempBaseDirectory;

	size_t trianglesCount;

};
