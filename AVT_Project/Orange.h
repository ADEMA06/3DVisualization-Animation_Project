#ifndef __ORANGE_H__
#define __ORANGE_H__

#define _USE_MATH_DEFINES
#include "GameObject.h"
#include <sstream>
#include <string>
#include "geometry.h"
#include "AVTmathLib.h"
#include "VSShaderlib.h"
#include <random>
#include <cmath>

class Orange : public GameObject {
    struct MyMesh stalk;
    struct MyMesh sphere;
    vec4 sphere_color;
    vec4 stalk_color;
    int speed_level;
    float radius;
	float dirRotation;
	bool is_moving;
	float orange_transformations[16];

public:
    Orange(vec3 position, vec4 sphere_color, vec4 stalk_color, float radius, float speed, float dirAngle) : GameObject(position, speed, dirAngle) {
        this->speed_level = 0;
        this->sphere_color = sphere_color;
        this->stalk_color = stalk_color;
        this->radius = radius;
		this->is_moving = true;

		vec3 min_pos = vec3(getPosition().x - radius, getPosition().y - radius, getPosition().z - radius);
		vec3 max_pos = vec3(getPosition().x + radius, getPosition().y + radius, getPosition().z + radius);
		setBoundingBox(min_pos, max_pos);
		 
		this->dirRotation = std::atan(speed*cos(getDirAngle()) / speed*sin(getDirAngle()));
    }

	void updatePosition(vec3 tablePos, float tableWidth, float tableHeight, float dt) {
		setRotAngle(getRotAngle() + rotationSpeed() * dt);
		vec3 speed_vector = vec3(getSpeed() * cos(getDirAngle()) * dt, 0, getSpeed() * sin(getDirAngle()) * dt);
		setPosition(getPosition() + speed_vector);
		int offset[2] = { -1, 1 };
		updateBoundingBox(speed_vector);
		if (getPosition().x > tablePos.x + tableWidth / 2 || getPosition().x < tablePos.x - tableWidth / 2 || getPosition().z > tablePos.z + tableHeight / 2 || getPosition().z < tablePos.z - tableHeight / 2) {
			int j = rand() % 2;
			vec3 position = vec3(offset[j] * rand() % ((int)tableWidth/2), 0.0f, offset[j] * (rand() % ((int)tableHeight/2)));
			int angle = rand() % 360;
			setDirAngle(angle);
			setPosition(position);
			vec3 min_pos = vec3(getPosition().x - radius / 2, getPosition().y - radius / 2, getPosition().z - radius / 2);
			vec3 max_pos = vec3(getPosition().x + radius / 2, getPosition().y + radius / 2, getPosition().z + radius / 2);
			setBoundingBox(min_pos, max_pos);
		}
	}

	float rotationSpeed() {
		float speedIntensity = std::sqrt(std::pow(getSpeed()*cos(getRotAngle()), 2) + std::pow(0, 2) + std::pow(getSpeed()*sin(getRotAngle()), 2));
		return (speedIntensity*57.29 / radius);
	}

	

    void createOrange() {
        float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
        float sphere_diff[] = { sphere_color.x, sphere_color.y, sphere_color.z, 0.6f };
        float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        float shininess = 100.0f;
        int texcount = 2;
        vec3 sphere_pos = getPosition() +  vec3(0,radius,0);
        sphere = createSphere(radius, 100);
        sphere = setMesh(sphere, amb, sphere_diff, spec, emissive, shininess, texcount, sphere_pos);
		sphere.rotAngle = 10.0f;
        float stalk_diff[] = { stalk_color.x, stalk_color.y, stalk_color.z, stalk_color.w };
        vec3 stalk_pos = sphere_pos + vec3(0, radius, 0);
		texcount = 0;
        stalk = createCylinder(radius/2, 0.1f, 20);
        stalk = setMesh(stalk, amb, stalk_diff, spec, emissive, shininess, texcount, stalk_pos);
    }

	void sphereTransformations() {
		setIdentityMatrix(orange_transformations);
		translate(MODEL, getPosition().x, getPosition().y + radius, getPosition().z);
		float speed_scalar = getSpeed();
		vec3 speed = vec3(speed_scalar * cos(getDirAngle()), 0.0f, speed_scalar * sin(getDirAngle()));
		float mag = sqrt(speed.x * speed.x + speed.y * speed.y + speed.z * speed.z);

		speed.x /= mag;
		speed.y /= mag;
		speed.z /= mag;
		vec3 res;
		res.x = speed.y * 0.0f - 1.0f * speed.z;
		res.y = speed.z * 0.0f - 0.0f * speed.x;
		res.z = speed.x * 1.0f - 0.0f * speed.y;
		rotate(MODEL, -getRotAngle(), res.x, res.y, res.z);
		multMatrix(orange_transformations, get(MODEL));
		translate(MODEL, 0, 0, 0);
	}

	void stalkTransformations() {
		multMatrix(MODEL, orange_transformations);
		translate(MODEL, 0, radius, 0);
	}

	void drawOrange(VSShaderLib shader, GLint pvm_uniformId, GLint vm_uniformId, GLint normal_uniformId, GLint lPos_uniformId) {
		//----------------Sphere-------------------
		setShaders(shader, sphere);
		pushMatrix(MODEL);
		sphereTransformations();
		drawMesh(sphere, shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
		popMatrix(MODEL);
		//-----------------------------------------
		
		//----------------Stalk--------------------
		setShaders(shader, stalk);
		pushMatrix(MODEL);
		stalkTransformations();
		drawMesh(stalk, shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
		popMatrix(MODEL);
		//-----------------------------------------
	}

	AABB getBoundingBox() {
		return GameObject::getBoundingBox();
	}
};

#endif
