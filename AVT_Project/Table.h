#ifndef __TABLE_H__
#define __TABLE_H__

#include <string>
#include <fstream>
#include "MeshBuilder.h"
#include "AssimpMesh.h"
#include "StaticObject.h"

extern char model_dir[50];

class Table : public StaticObject {
	//Meshes
	struct MyMesh base;
	std::vector<struct MyMesh> legs;
	std::vector<struct MyMesh> meshes;
	const aiScene* tableScene = NULL;

	//Dimensions
	float width;
	float height;
	float thickness;
	float legSide;
	float legHeight;


public:

	Table(float width, float height, float thickness, float legSide, float legHeight, vec3 position) : StaticObject(position) {
		this->height = height;
		this->width = width;
		this->thickness = thickness;
		this->legSide = legSide;
		this->legHeight = legHeight;
	}

	void createTable(GLuint *textures, int offset) {
		MeshBuilder builder;

		float amb[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float diff[] = { 0.6f, 0.6f, 0.6f, 1.0f };
		float spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 500.0f;
		int texcount = 3;
		vec3 table_pos = getPosition();
		base = createCube();
		base = builder.setMesh(base, amb, diff, spec, emissive, shininess, texcount, table_pos);
		MyMesh amesh;
		for (int i = -1; i <= 1; i+=2) {
			for(int j = -1; j <= 1; j+=2) {
				amesh = createCube();
				vec3 position = table_pos + vec3(j*width/2 - j*legSide/2, -legHeight, i * height / 2 - i * legSide/2);
				amesh = builder.setMesh(amesh, amb, diff, spec, emissive, shininess, texcount, position);
				legs.push_back(amesh);
			}
		}
		std::string filepath;
		strcpy(model_dir, "Volcano");
		while (true) {

			std::ostringstream oss;
			oss << model_dir << "/" << model_dir << ".obj";;
			filepath = oss.str();   //path of OBJ file in the VS project

			strcat(model_dir, "/");
			//check if file exists
			std::ifstream fin(filepath.c_str());
			if (!fin.fail()) {
				fin.close();
				break;
			}
			else
				printf("Couldn't open file: %s\n", filepath.c_str());
		}
		tableScene = Import3DFromFile(filepath, tableScene, &vec3(), &vec3());
		meshes = createMeshFromAssimp(tableScene, textures, offset);

	}

	void tableRecursiveDraw(aiNode* nd, VSShaderLib* shader, GLuint *textures, int offset) {
		MeshBuilder builder;
		GLint diffMapCount_loc = glGetUniformLocation(shader->getProgramIndex(), "diffMapCount");
		for (int n = 0; n < meshes.size(); ++n) {
			int diffMapCount = 0;
			glUniform1ui(diffMapCount_loc, 0);
			builder.setShaders(shader, meshes[n]);
			if (meshes[n].mat.texCount != 0) {
				for (unsigned int i = 0; i < meshes[n].mat.texCount; ++i) {
					if (meshes[n].texTypes[i] == DIFFUSE) {
						if (diffMapCount == 0) {
							diffMapCount++;
							glActiveTexture(GL_TEXTURE0 + offset);
							glBindTexture(GL_TEXTURE_2D, textures[offset]);
							GLint loc = glGetUniformLocation(shader->getProgramIndex(), "texUnitDiff");
							glUniform1i(loc, meshes[n].texUnits[i]);
							glUniform1i(diffMapCount_loc, diffMapCount);
						}
					}
				}
			}
			builder.drawMesh(meshes[n], shader);
		}
	}

	void drawTable(VSShaderLib *shader, GLuint *textures, int offset) {
		MeshBuilder builder;
		builder.setShaders(shader, base);
		pushMatrix(MODEL);
		translate(MODEL, -width/2, -thickness, -height/2);
		translate(MODEL, getPosition().x, getPosition().y, getPosition().z);
		scale(MODEL, width, thickness, height);
		builder.drawMesh(base, shader);
		popMatrix(MODEL);

		pushMatrix(MODEL);
		translate(MODEL, width / 3, 0.0f, width/3);
		scale(MODEL, 10.0f, 10.0f, 10.0f);
		tableRecursiveDraw(tableScene->mRootNode, shader, textures, offset);
		popMatrix(MODEL);


		for (int i = 0; i < legs.size(); i++) {
			builder.setShaders(shader, legs.at(i));
			pushMatrix(MODEL);
			translate(MODEL, -legSide/2, -thickness, -legSide/2);
			translate(MODEL, legs.at(i).position.x, legs.at(i).position.y, legs.at(i).position.z);
			scale(MODEL, legSide, legHeight, legSide);
			builder.drawMesh(legs.at(i), shader);
			popMatrix(MODEL);
		}
	}
};

#endif