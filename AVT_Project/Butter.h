#ifndef __BUTTER_H__
#define __BUTTER_H__

#include <sstream>
#include <string>
#include "geometry.h"
#include "AVTmathLib.h"
#include "VSShaderlib.h"
#include "GameObject.h"

const float butter_width = 2.0f;
const float butter_height = 0.75f;
const float butter_thickness = 0.75f;

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

class Butter : public GameObject {
    struct MyMesh butter_body;
    struct MyMesh butter_foil;
    vec4 foil_color;
	float accel;
	vec3 colliding_speed;


public:
    Butter(vec3 position, vec4 foil_color) : GameObject(position) {
        this->foil_color = foil_color;
		vec3 min_pos = vec3(getPosition().x - butter_width / 2, getPosition().y - butter_height / 2, getPosition().z - butter_thickness / 2);
		vec3 max_pos = vec3(getPosition().x + butter_width / 2, getPosition().y + butter_height / 2, getPosition().z + butter_thickness / 2);
		setBoundingBox(min_pos, max_pos);
		accel = 0;

	}

	void createButter() {
		float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
		float foil_diff[] = { foil_color.x, foil_color.y, foil_color.z, 0.5f};
		float butter_diff[] = { 0.92f, 0.90f, 0.16f, 0.5f };
		float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 100.0f;
		int texcount = 0;
		vec3 butter_pos = getPosition();
		butter_body = createCube();
		butter_body = setMesh(butter_body, amb, butter_diff, spec, emissive, shininess, texcount, butter_pos);
		butter_foil = createCube();
		butter_foil = setMesh(butter_foil, amb, foil_diff, spec, emissive, shininess, texcount, butter_pos);
	}

	void butterBodyTransformations() {
		translate(MODEL, -butter_width / 2, 0, -butter_thickness / 2);
		translate(MODEL, getPosition().x, getPosition().y, getPosition().z);
		scale(MODEL, butter_width, butter_height, butter_thickness);
	}

	void butterFoilTransformations() {
		translate(MODEL, -butter_width / 2, 0, -butter_thickness / 2);
		translate(MODEL, getPosition().x - 0.3f, getPosition().y, getPosition().z - 0.001f);
		scale(MODEL, butter_width, butter_height * 1.01f, butter_thickness * 1.01f);
	}

	void drawButter(VSShaderLib shader, GLint pvm_uniformId, GLint vm_uniformId, GLint normal_uniformId, GLint lPos_uniformId) {
		setShaders(shader, butter_body);
		pushMatrix(MODEL);
		butterBodyTransformations();
		drawMesh(butter_body, shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
		popMatrix(MODEL);


		setShaders(shader, butter_foil);
		pushMatrix(MODEL);
		butterFoilTransformations();
		drawMesh(butter_foil, shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
		popMatrix(MODEL);
	}

	void update(float dt) {
		colliding_speed = colliding_speed + colliding_speed.normalize() * accel * dt;
		setSpeed(getSpeed() + accel * dt);
		if (getSpeed() > 0) {
			vec3 offset = colliding_speed * dt;
			updateBoundingBox(offset);
			setPosition(getPosition() + offset);
		}

		else {
			setSpeed(0.0f);
			accel = 0;
			colliding_speed = vec3(0.0f, 0.0f, 0.0f);
		}
	}

	void setCollidingSpeed(vec3 col_speed) {
		colliding_speed = col_speed;
	}

	void setAccel(float a) {
		accel = a;
	}


	AABB getBoundingBox() {
		return GameObject::getBoundingBox();
	}
};

#endif
