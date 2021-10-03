#ifndef __CHEERIO_H__
#define __CHEERIO_H__

#include "GameObject.h"
#include <sstream>
#include <string>
#include "geometry.h"
#include "AVTmathLib.h"
#include "VSShaderlib.h"

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

class Cheerio : GameObject {
	struct MyMesh body;
	vec4 body_color;

public:
	Cheerio(vec3 position, vec4 body_color) : GameObject(position) {
		this->body_color = body_color;
	}

	void createCheerio() {
		float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
		float diff[] = { body_color.x, body_color.y, body_color.z, body_color.w };
		float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 10.0f;
		int texcount = 0;
		vec3 body_pos = getPosition();
		body = createTorus(0.2f, 0.5f, 20, 100);
		body = setMesh(body, amb, diff, spec, emissive, shininess, texcount, body_pos);
	}

	void drawCheerio(VSShaderLib shader, GLint pvm_uniformId, GLint vm_uniformId, GLint normal_uniformId, GLint lPos_uniformId) {
		GLint loc;
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, body.mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, body.mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, body.mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, body.mat.shininess);
		pushMatrix(MODEL);
		translate(MODEL, getPosition().x, getPosition().y + 0.15f, getPosition().z);
		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// Render mesh
		glBindVertexArray(body.vao);

		if (!shader.isProgramValid()) {
			printf("Program Not Valid!\n");
			exit(1);
		}
		glDrawElements(body.type, body.numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);
	}

};

#endif