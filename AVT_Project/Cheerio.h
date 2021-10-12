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
		body = createTorus(0.1f, 0.3f, 10, 10);
		body = setMesh(body, amb, diff, spec, emissive, shininess, texcount, body_pos);
	}

	void bodyTransformations() {
		mMatrix[MODEL][12] = getPosition().x;
		mMatrix[MODEL][13] = getPosition().y + 0.15f;
		mMatrix[MODEL][14] = getPosition().z;
	}

	void drawCheerio(VSShaderLib shader, GLint pvm_uniformId, GLint vm_uniformId, GLint normal_uniformId, GLint lPos_uniformId) {
		GLint loc;
		setShaders(shader, body);
		pushMatrix(MODEL);
		bodyTransformations();
		drawMesh(body, shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
		popMatrix(MODEL);
	}

};

#endif