
#include "OBJLoader.h"

#define MAX_LINE_SIZE 1024
#define GET_VERTEX(vector, index) index > 0 ? vector[index-1] : vector[vector.size()+index]

using namespace OBJ;

void OBJLoader::parseFace(char** context, IOBJLoaderListener* listener) {
	unsigned int verticesProcessed = 0;
	char* vertexToken;

	OBJTriangle t;

	while(true) { 
		vertexToken = strtok_s(NULL, " \n\r\t", context);

		if (vertexToken == NULL) break;

		unsigned int storePosition = verticesProcessed < 2 ? verticesProcessed : 2;

		if (verticesProcessed > 2) {
			t.position[1] = t.position[2]; 
			t.normal[1] = t.normal[2]; 
			t.texCoords[1] = t.texCoords[2]; 
		}

		int idx = atoi(vertexToken);
		t.position[storePosition] = GET_VERTEX(verticesBuffer, idx);

		// try to find slash (after it will be texture coords index) 
		char* slash = strchr(vertexToken,'/');

		if (slash != NULL) {
			// if there is another slash, it means that only normal is specified (tex coords are ommited)
			if (slash[1] == '/') {
				// process normal
				// do nothing here
			} else {
				// load texture coords index to y field
				idx = atoi(slash + 1);
				t.texCoords[storePosition] = GET_VERTEX(textureCoordinatesBuffer, idx);
			}

			// try to find another slash (after it will be normal index) 
			slash = strchr(slash+1,'/');

			if (slash != NULL) {
				// load texture coords index to z field
				idx = atoi(slash + 1);
				t.normal[storePosition] = GET_VERTEX(normalsBuffer, idx); 
			}
		}

		verticesProcessed++;

		if (verticesProcessed > 2) {
			listener->newTriangle(t);
		}
	}

	if (verticesProcessed < 3) {
		//TRACE_WARNING("WARNING: loading of faces with " << verticesProcessed << " vertices is not supprted." << std::endl);
	}

}

void OBJLoader::newToken(const char* token, char** context, IOBJLoaderListener* listener) {

		OBJVec3 temp;

		if (strcmp(token, "v") == 0) {
			// Specifies a geometric vertex and its x y z coordinates. Rational
			// curves and surfaces require a fourth homogeneous coordinate, also
			// called the weight.

			// v x y z w

			parseVec3(temp, context);
			verticesBuffer.push_back(temp);

		} else if (strcmp(token, "vn") == 0) {
			// Specifies a normal vector with components i, j, and k.

			// vn i j k
			parseVec3(temp, context);
			normalsBuffer.push_back(temp);
		} else if (strcmp(token, "vt") == 0) {
			// Specifies a texture vertex and its coordinates.

			// vt u v w
			parseVec3(temp, context);

			textureCoordinatesBuffer.push_back(temp);
		} else if (strcmp(token, "f") == 0) {
			// Specifies a face element and its vertex reference number. You can
			// optionally include the texture vertex and vertex normal reference
			// numbers.

			// f  v1/vt1/vn1   v2/vt2/vn2   v3/vt3/vn3 . . .
			parseFace(context, listener);

		} else if (strcmp(token, "lp") == 0) {
			// Point light
			int vertexIndex = parseInt(context);

			if (vertexIndex == INT_MAX) {
				// Error - no vertex index specified
			} else {
				listener->newPointLight(GET_VERTEX(verticesBuffer, vertexIndex));
			}

		} else if (strcmp(token, "lq") == 0) {
			// Quad light
			int vertexIndexA = parseInt(context);
			int vertexIndexB = parseInt(context);
			int vertexIndexC = parseInt(context);
			int vertexIndexD = parseInt(context);

		} else if (strcmp(token, "c") == 0) {

			// Camera
			int fromIndex = parseInt(context);
			int toIndex = parseInt(context);
			int upVectorIndex = parseInt(context);

			if (fromIndex == INT_MAX || toIndex == INT_MAX || upVectorIndex == INT_MAX) {
				//TRACE_WARNING("WARNING: obj loader encountered problem while parsing camera. Not all parameters specified (needed 3 indices)." << std::endl);
			} else {

				OBJCamera camera;
				camera.from = GET_VERTEX(verticesBuffer, fromIndex);
				camera.at = GET_VERTEX(verticesBuffer, toIndex);
				camera.up = GET_VERTEX(verticesBuffer, upVectorIndex);

				listener->newCamera(camera);
			}
		} else if (strcmp(token, "g") == 0) {
			// Specifies the group name for the elements that follow it. You can
			// have multiple group names.

			// g group_name1 group_name2 . . .

		} else if (strcmp(token, "s") == 0) {
			// Sets the smoothing group for the elements that follow it.

		} else if (strcmp(token, "usemtl") == 0) {
			// Specifies the material name for the element following it.

			// usemtl material_name

			// get material name
			token = strtok_s(NULL, " \n\r\t", context);
			if (token == NULL) return;

			listener->useMaterial(token);

		} else if (strcmp(token, "mtllib") == 0) {
			// Specifies the material library file for the material definitions set with the usemtl statement.
			// You can specify multiple filenames with mtllib.

			// retrieve material file name from file
			token = strtok_s(NULL, " \n\r\t", context);

			if (token == NULL) return;

			char filePath[2*256];

			strcpy_s(filePath, "assets/");
			strcat_s(filePath, token);

			parseMtlFile(filePath, listener);
		} else {
			listener->Error(OBJError::UNKNOWN_COMMAND, token);
		}
}

void OBJLoader::parseMtlFile(const char* fileName, IOBJLoaderListener* listener) {

	FILE* file = NULL;
	fopen_s(&file, fileName, "r");

	if (file == NULL) {
		return;
	}

	char lineBuffer[MAX_LINE_SIZE];
	char mtlNameBuffer[MAX_LINE_SIZE];
	char diffuseTextureBuffer[MAX_LINE_SIZE];
	char* context;

	OBJMaterial* currentMaterial = NULL;

	while(fgets(lineBuffer, MAX_LINE_SIZE, file) != NULL) {

		char* token = strtok_s(lineBuffer, " \n\r\t", &context);

		if (lineBuffer[0] == '#') continue;

		while (token != NULL) {

			if (strcmp(token, "newmtl") == 0) {
				// Create new material
				token = strtok_s(NULL, " \n\r\t", &context);

				if (currentMaterial) {
					// emit material
					listener->newMaterial(*currentMaterial);
				} else {
					currentMaterial = (OBJMaterial*) alloca(sizeof(OBJMaterial));

				}

				memset(currentMaterial, 0, sizeof(OBJMaterial));

				// set default transmittance to 1.0f
				currentMaterial->transmittance = 1.0f;

				strcpy_s(mtlNameBuffer, token);
				currentMaterial->materialName = mtlNameBuffer;

			} else if (strcmp(token, "Ka") == 0) {
				// The Ka statement specifies the ambient reflectivity of the current material
				// Ka r g b                        - as RGB values
				// Ka spectral file.rfl factor     - using a spectral curve.
				// Ka xyz x y z                    - in CIEXYZ color space

			} else if (strcmp(token, "Kd") == 0) {
				// The Kd statement specifies the diffuse reflectivity of the current material
				// Kd r g b                        - as RGB values
				// Kd spectral file.rfl factor     - using a spectral curve.
				// Kd xyz x y z                    - in CIEXYZ color space

				parseVec3(currentMaterial->diffuseColor, &context);
			} else if (strcmp(token, "Ks") == 0) {
				// The Ks statement specifies the specular reflectivity of the current material
				// Ks r g b                        - as RGB values
				// Ks spectral file.rfl factor     - using a spectral curve.
				// Ks xyz x y z                    - in CIEXYZ color space

			} else if (strcmp(token, "Ke") == 0) {
				// The Ke statement specifies the the emissive color
				// Ke r g b                        - as RGB values
				// Ke spectral file.rfl factor     - using a spectral curve.
				// Ke xyz x y z                    - in CIEXYZ color space
				parseVec3(currentMaterial->emissiveColor, &context);
			} else if (strcmp(token, "Tf") == 0) {
				// The Tf statement specifies the transmission filter. 
				//      Any light passing through the object is filtered by the transmission 
				//      filter, which only allows the specifiec colors to pass through.  For 
				//      example, Tf 0 1 0 allows all the green to pass through and filters out 
				//      all the red and blue.

				// Tf r g b                        - as RGB values
				// Tf spectral file.rfl factor     - using a spectral curve.
				// Tf xyz x y z                    - in CIEXYZ color space

			} else if (strcmp(token, "illum") == 0) {
				// The "illum" statement specifies the illumination model to use in the 
				// material.  Illumination models are mathematical equations that represent 
				// various material lighting and shading effects.


				// illum illum_#
				// See documentation for explanation of values

			} else if (strcmp(token, "d") == 0 || strcmp(token, "Tr") == 0) {
				// Specifies the dissolve for the current material.
				// "factor" is the amount this material dissolves into the background.  A 
				// factor of 1.0 is fully opaque. 

				// *some formats use Tr instead of d

				// d factor
				// d -halo factor   - Specifies that a dissolve is dependent on the surface orientation relative to the viewer

				currentMaterial->transmittance = parseFloat(&context);

			} else if (strcmp(token, "Ns") == 0) {
				// Specifies the specular exponent for the current material.  This defines 
				// the focus of the specular highlight.

				currentMaterial->shine = parseFloat(&context);

			} else if (strcmp(token, "sharpness") == 0) {
				// Specifies the sharpness of the reflections from the local reflection map.

				// sharpness value

			} else if (strcmp(token, "Ni") == 0) {
				// Specifies the optical density for the surface.  This is also known as 
				// index of refraction.

				currentMaterial->IOR = parseFloat(&context);

			} else if (strcmp(token, "map_Ka") == 0 || strcmp(token, "map_kA") == 0) {
				// Specifies that a color texture file or a color procedural texture file 
				// is applied to the ambient reflectivity of the material.

				// map_Ka -options args filename

			} else if (strcmp(token, "map_Kd") == 0) {
				// Specifies that a color texture file or a color procedural texture file 
				// is applied to the diffuse reflectivity of the material.

				// map_Kd -options args filename

				// retrieve texture file name from file
				token = strtok_s(NULL, " \n\r\t", &context);

				if (token == NULL) break;

				strcpy_s(diffuseTextureBuffer, token);
				currentMaterial->diffuseTexture = diffuseTextureBuffer;

			} else if (strcmp(token, "map_Ks") == 0 || strcmp(token, "map_kS") == 0) {
				// Specifies that a color texture file or a color procedural texture file 
				// is applied to the specular reflectivity of the material.

				// map_Ks -options args filename

			} else if (strcmp(token, "map_Ns") == 0) {
				// Specifies that a scalar texture file or scalar procedural texture file 
				// is linked to the specular exponent of the material.  During rendering, 
				// the map_Ns value is multiplied by the Ns value.

				//  map_Ns -options args filename

			} else if (strcmp(token, "map_d") == 0 || strcmp(token, "map_opacity") == 0) {
				// Specifies that a scalar texture file or scalar procedural texture file 
				// is linked to the dissolve of the material.  During rendering, the map_d 
				// value is multiplied by the d value.

				// map_d -options args filename

			} else if (strcmp(token, "decal") == 0) {
				// Specifies that a scalar texture file or a scalar procedural texture 
				// file is used to selectively replace the material color with the texture color.

				// decal -options args filename

			} else if (strcmp(token, "disp") == 0) {
				// Specifies that a scalar texture is used to deform the surface of an 
				// object, creating surface roughness.

				// disp -options args filename

			} else if (strcmp(token, "bump") == 0 || strcmp(token, "map_bump") == 0) {
				// Specifies that a bump texture file or a bump procedural texture file is 
				// linked to the material.

				// bump -options args filename

			} else if (strcmp(token, "refl") == 0) {
				// The following syntax statements describe the reflection map

			}else {
				//TRACE_WARNING("WARNING: mtl loader encountered unknown command '" << command << "'"<< endl);
			}

			token = strtok_s(NULL, " ", &context);

		}
	}

	// emit last material
	listener->newMaterial(*currentMaterial);

	fclose(file);
}

void OBJLoader::loadFromFile(const char* fileName, IOBJLoaderListener* listener) {
		verticesBuffer.clear();
		normalsBuffer.clear();
		textureCoordinatesBuffer.clear();

		FILE* file = NULL;
		fopen_s(&file, fileName, "r");

		if (file == NULL) {
			return;
		}

		char lineBuffer[MAX_LINE_SIZE];
		char* context;

		while(fgets(lineBuffer, MAX_LINE_SIZE, file) != NULL) {

			char* token = strtok_s(lineBuffer, " \n\r\t", &context);

			if (lineBuffer[0] == '#') continue;

			while (token != NULL) {
				newToken(token, &context, listener);
				token = strtok_s(NULL, " \n\r\t", &context);
			}
		}

		fclose(file);
}

#undef MAX_LINE_SIZE
#undef GET_VERTEX