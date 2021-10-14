#ifndef __TABLE_H__
#define __TABLE_H__

#pragma once

#include <sstream>
#include <string>
#include "geometry.h"
#include "AVTmathLib.h"
#include "VSShaderlib.h"
#include "GameObject.h"

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

class Table : GameObject{
	float width;
	float height;
	float thickness;
	float legSide;
	float legHeight;
	struct MyMesh base;
	std::vector<struct MyMesh> legs;


public:

	Table(float width, float height, float thickness, float legSide, float legHeight, vec3 position) : GameObject(position) {
		this->height = height;
		this->width = width;
		this->thickness = thickness;
		this->legSide = legSide;
		this->legHeight = legHeight;
	}

	void createTable() {
		float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
		float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
		float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 500.0f;
		int texcount = 3;
		vec3 table_pos = getPosition();
		base = createCube();
		base = setMesh(base, amb, diff, spec, emissive, shininess, texcount, table_pos);
		MyMesh amesh;
		for (int i = -1; i <= 1; i+=2) {
			for(int j = -1; j <= 1; j+=2) {
				amesh = createCube();
				vec3 position = table_pos + vec3(j*width/2 - j*legSide/2, -legHeight, i * height / 2 - i * legSide/2);
				amesh = setMesh(amesh, amb, diff, spec, emissive, shininess, texcount, position);
				legs.push_back(amesh);
			}
		}
	}

	void drawTable(VSShaderLib shader, GLint pvm_uniformId, GLint vm_uniformId, GLint normal_uniformId, GLint lPos_uniformId) {
		GLint loc;
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, base.mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, base.mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, base.mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, base.mat.shininess);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.texCount");
		glUniform1i(loc, base.mat.texCount);
		pushMatrix(MODEL);
		translate(MODEL, -width/2, -thickness, -height/2);
		translate(MODEL, getPosition().x, getPosition().y, getPosition().z);
		scale(MODEL, width, thickness, height);
		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// Render mesh
		glBindVertexArray(base.vao);

		if (!shader.isProgramValid()) {
			printf("Program Not Valid!\n");
			exit(1);
		}
		glDrawElements(base.type, base.numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);


		for (int i = 0; i < legs.size(); i++) {
	
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
			glUniform4fv(loc, 1, legs.at(i).mat.ambient);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, legs.at(i).mat.diffuse);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
			glUniform4fv(loc, 1, legs.at(i).mat.specular);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
			glUniform1f(loc, legs.at(i).mat.shininess);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.texCount");
			glUniform1i(loc, base.mat.texCount);
			pushMatrix(MODEL);
			translate(MODEL, -legSide/2, -thickness, -legSide/2);
			translate(MODEL, legs.at(i).position.x, legs.at(i).position.y, legs.at(i).position.z);
			scale(MODEL, legSide, legHeight, legSide);
			

			// send matrices to OGL
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			// Render mesh
			glBindVertexArray(legs.at(i).vao);

			if (!shader.isProgramValid()) {
				printf("Program Not Valid!\n");
				exit(1);
			}
			glDrawElements(legs.at(i).type, legs.at(i).numIndexes, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			popMatrix(MODEL);
		}
	}
};

#endif