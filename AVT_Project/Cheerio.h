#ifndef __CHEERIO_H__
#define __CHEERIO_H__

#include "GameObject.h"
#include <sstream>
#include <string>
#include "geometry.h"
#include "AVTmathLib.h"
#include "VSShaderlib.h"
#include "InducedMovementObject.h"

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];
float outer_radius = 0.4;

class Cheerio : public InducedMovementObject {
	struct MyMesh* body;
	vec4 body_color;
	vec3 position;


public:
	Cheerio(vec3 position, vec4 body_color) : InducedMovementObject(position) {
		this->body_color = body_color;
		vec3 min_pos = vec3(getPosition().x - outer_radius, getPosition().y, getPosition().z - outer_radius);
		vec3 max_pos = vec3(getPosition().x + outer_radius, getPosition().y, getPosition().z + outer_radius);
		setAccel(0.0f);
		setBoundingBox(min_pos, max_pos);
	}

	Cheerio(vec3 position, vec4 body_color, MyMesh* mesh) : InducedMovementObject(position) {
		this->body_color = body_color;
		body = mesh;
		this->position = position;
		vec3 min_pos = vec3(getPosition().x - outer_radius, getPosition().y, getPosition().z - outer_radius);
		vec3 max_pos = vec3(getPosition().x + outer_radius, getPosition().y, getPosition().z + outer_radius);
		setAccel(0.0f);
		setBoundingBox(min_pos, max_pos);
	}

	void bodyTransformations() {
		mMatrix[MODEL][12] = getPosition().x;
		mMatrix[MODEL][13] = getPosition().y + 0.15f;
		mMatrix[MODEL][14] = getPosition().z;
	}

	/*void drawCheerio(VSShaderLib& shader, MyMesh* mesh) {
		GLint loc;
		setShaders(shader, mesh);
		pushMatrix(MODEL);
		bodyTransformations();
		drawMesh(mesh, shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
		popMatrix(MODEL);
	}*/

	AABB getBoundingBox() {
		return GameObject::getBoundingBox();
	}
};

#endif