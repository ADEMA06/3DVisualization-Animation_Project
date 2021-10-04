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
#include <math.h>

class Orange : public GameObject {
    struct MyMesh stalk;
    struct MyMesh sphere;
    vec4 sphere_color;
    vec4 stalk_color;
    int speed_level;
    float radius;
	float dirRotation;
	bool is_moving;

public:
    Orange(vec3 position, vec4 sphere_color, vec4 stalk_color, float radius, vec3 speed) : GameObject(position, speed) {
        this->speed_level = 0;
        this->sphere_color = sphere_color;
        this->stalk_color = stalk_color;
        this->radius = radius;
		this->is_moving = true;

		this->dirRotation = std::atan(speed.x / speed.z);
    }

	void updatePosition(vec3 tablePos, float tableWidth, float tableHeight, float dt) {
		setPosition(getPosition() + vec3(getSpeed().x*dt, getSpeed().y * dt, getSpeed().z * dt));
		setRotAngle(getRotAngle() + rotationSpeed()*dt);
		int offset[2] = { -1, 1 };
		if (getPosition().x > tablePos.x + tableWidth / 2 || getPosition().x < tablePos.x - tableWidth / 2 || getPosition().y > tablePos.y + tableHeight / 2 || getPosition().y < tablePos.y - tableHeight / 2) {
			int j = rand() % 2;
			vec3 position = vec3(offset[j] * rand() % ((int)tableWidth/2), 0.0f, offset[j] * (rand() % ((int)tableHeight/2)));
			setPosition(position);
		}
	}

	float rotationSpeed() {
		float speedIntensity = std::sqrt(std::pow(getSpeed().x, 2) + std::pow(getSpeed().y, 2) + std::pow(getSpeed().z, 2));
		return (speedIntensity*57.29 / radius);
	}

	

    void createOrange() {
        float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
        float sphere_diff[] = { sphere_color.x, sphere_color.y, sphere_color.z, sphere_color.w };
        float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        float shininess = 100.0f;
        int texcount = 0;
        vec3 sphere_pos = getPosition() +  vec3(0,radius,0);
        sphere = createSphere(radius, 100);
        sphere = setMesh(sphere, amb, sphere_diff, spec, emissive, shininess, texcount, sphere_pos);
		sphere.rotAngle = 10.0f;
        float stalk_diff[] = { stalk_color.x, stalk_color.y, stalk_color.z, stalk_color.w };
        vec3 stalk_pos = sphere_pos + vec3(0, radius, 0);
        stalk = createCylinder(radius/2, 0.1f, 20);
        stalk = setMesh(stalk, amb, stalk_diff, spec, emissive, shininess, texcount, stalk_pos);
    }

	void sphereTransformations() {
		translate(MODEL, getPosition().x, getPosition().y + radius, getPosition().z);
		rotate(MODEL, getRotAngle(), 0.0f, 0.0f, 1.0f);
		translate(MODEL, 0, 0, 0);
	}

	void stalkTransformations() {
		translate(MODEL, getPosition().x, getPosition().y + radius, getPosition().z);
		//rotate(MODEL, dirRotation, 0.0f, 0.0f, 1.0f);
		//rotate(MODEL, -getRotAngle(), 0.0f, 0.0f, 1.0f);

		//Logica da rotacao da laranja
		//Pego no vetor da velocidade, encontro um vetor perpendicular no mesmo plano, esse vai ser o eixo de rotacao,
		//e simplesmente faco o rotae com o rotAngle nesse eixo. So tem o caso limite quando rodas 90 graus para cima,
		//o que e meio weird
		vec3 speed = getSpeed();
		float mag = sqrt(speed.x * speed.x + speed.y * speed.y + speed.z * speed.z);

		speed.x /= mag;
		speed.y /= mag;
		speed.z /= mag;
		vec3 res;
		res.x = speed.y * 0.0f - 1.0f * speed.z;
		res.y = speed.z * 0.0f - 0.0f * speed.x;
		res.z = speed.x * 1.0f - 0.0f * speed.y;
		rotate(MODEL, -getRotAngle(), res.x, res.y, res.z);
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
};

#endif
