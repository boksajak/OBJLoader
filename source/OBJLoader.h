//----------------------------------------------------------------------------------------
/**
* \file       OBJLoader.h
* \author     Jakub Bokšanský
* \date       2012/01/03
* \brief      Obj files loader class
*
*  This class implements loading obj files according to description at http://paulbourke.net/dataformats/obj/
*  and associated mtl files as described in http://paulbourke.net/dataformats/mtl/
*
*/
//----------------------------------------------------------------------------------------
#pragma once

#include <cfloat>
#include <vector>
#include <iostream>
#include <cstring>

namespace OBJ {

	struct OBJVec3 {
		OBJVec3() : x(0.0f), y(0.0f), z(0.0f) { }
		OBJVec3(float a) : x(a), y(a), z(a) { }
		OBJVec3(float x, float y, float z) : x(x), y(y), z(z) { }

		float x, y, z;
	};

	struct OBJMaterial {
		char* diffuseTexture;
		char* materialName;
		OBJVec3 diffuseColor;
		OBJVec3 emissiveColor;
		float transmittance;
		float shine;
		float IOR;
	};

	struct OBJTriangle {
		OBJVec3 position[3];
		OBJVec3 normal[3];
		OBJVec3 texCoords[3];
	};

	struct OBJCamera {
		OBJVec3 from;
		OBJVec3 at;
		OBJVec3 up;
	};

	enum OBJError {
		UNKNOWN_COMMAND
	};

	class IOBJLoaderListener {
	public:
		virtual void newTriangle(const OBJTriangle &triangle) = 0;
		virtual void newCamera(const OBJCamera &camera) = 0;
		virtual void newPointLight(OBJVec3 position) = 0;
		virtual void useMaterial(const char* name) = 0;
		virtual void newMaterial(const OBJMaterial &material) = 0;
		virtual void Error(const OBJError error, const char* data) = 0;
	};

	class OBJLoader {
	public:

		void loadFromFile(const char* fileName, IOBJLoaderListener* listener);

	private:
		void (*newTriangle)(OBJVec3 position, OBJVec3 normal, OBJVec3 texCoords);

		std::vector<OBJVec3> verticesBuffer;
		std::vector<OBJVec3> normalsBuffer;
		std::vector<OBJVec3> textureCoordinatesBuffer;

		void newToken(const char* token, char** context, IOBJLoaderListener* listener);

		void parseVec3(OBJVec3 &result, char** context) {
			result.x = 0.0f;
			result.y = 0.0f;
			result.z = 0.0f;

			char* token;

			// get x
			token = strtok_s(NULL, " \n\r\t", context);
			if (token != NULL) result.x = (float) atof(token);
			else return;

			// get y
			token = strtok_s(NULL, " \n\r\t", context);
			if (token != NULL) result.y = (float) atof(token);
			else return;

			// get z
			token = strtok_s(NULL, " \n\r\t", context);
			if (token != NULL) result.z = (float) atof(token);
		}

		int parseInt(char** context) {
			int intValue = INT_MAX;

			char* token = strtok_s(NULL, " \n\r\t", context);
			if (token != NULL) {
				intValue = atoi(token);
			}
			return intValue;
		}

		float parseFloat(char** context) {
			char* token;

			// get x
			token = strtok_s(NULL, " \n\r\t", context);
			if (token != NULL) return (float) atof(token);

			return FLT_MAX;
		}

		void parseFace(char** context, IOBJLoaderListener* listener);

		void parseMtlFile(const char* fileName, IOBJLoaderListener* listener);
	};
}