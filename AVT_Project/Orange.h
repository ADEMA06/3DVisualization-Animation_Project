#ifndef __ORANGE_H__
#define __ORANGE_H__

#define _USE_MATH_DEFINES
#include "MovableObject.h"
#include <string>
#include "MeshBuilder.h"
#include <random>

class Orange : public MovableObject {
	//Meshes and mesh details
    struct MyMesh stalk;
    struct MyMesh sphere;
    vec4 sphere_color;
    vec4 stalk_color;

	//Speed increase level
    int speed_level = 1;

	//Radius
    float radius;

	//Angle in which the angl is rotated around itself
	float own_axis_angle;

	//Orange transformation matrix
	float orange_transformations[16];

	//Cooldown after leaving table
	float countdown = 0;

public:
    Orange(vec3 position, vec4 sphere_color, vec4 stalk_color, float radius, float speed, float direction_angle) : MovableObject(position, speed, direction_angle) {
        this->sphere_color = sphere_color;
        this->stalk_color = stalk_color;
        this->radius = radius;

		vec3 min_pos = vec3(getPosition().x - radius, getPosition().y - radius, getPosition().z - radius);
		vec3 max_pos = vec3(getPosition().x + radius, getPosition().y + radius, getPosition().z + radius);
		setBoundingBox(min_pos, max_pos);
		 
		this->own_axis_angle = std::atan(speed*cos(own_axis_angle) / speed*sin(own_axis_angle));
    }

	void updatePosition(vec3 tablePos, float tableWidth, float tableHeight, float dt) {
		countdown = std::max(countdown - dt, 0.0f);
		own_axis_angle += rotationSpeed() * dt;
		vec3 speed_vector = getSpeedVector(dt);
		setPosition(getPosition() + speed_vector);
		int offset[2] = { -1, 1 };
		updateBoundingBox(speed_vector);
		if (getPosition().x > tablePos.x + tableWidth / 2 || getPosition().x < tablePos.x - tableWidth / 2 || getPosition().z > tablePos.z + tableHeight / 2 || getPosition().z < tablePos.z - tableHeight / 2) {
			int j = rand() % 2;
			vec3 position = vec3(offset[j] * rand() % ((int)tableWidth/2), 0.0f, offset[j] * (rand() % ((int)tableHeight/2)));
			int angle = rand() % 360;
			setDirectionAngle(angle);
			setPosition(position);
			vec3 min_pos = vec3(getPosition().x - radius / 2, getPosition().y - radius / 2, getPosition().z - radius / 2);
			vec3 max_pos = vec3(getPosition().x + radius / 2, getPosition().y + radius / 2, getPosition().z + radius / 2);
			countdown = 2.0f;
			setBoundingBox(min_pos, max_pos);
		}
	}

	void updateSpeed(int t) {
		setSpeed(getSpeed() / speed_level);
		speed_level = std::min((t / 100000) + 1, 6);
		setSpeed(getSpeed() * speed_level);
	}

	float rotationSpeed() {
		float speedIntensity = std::sqrt(std::pow(getSpeed()*cos(getDirectionAngle()), 2) + std::pow(getSpeed()*sin(getDirectionAngle()), 2));
		return (speedIntensity*57.29 / radius);
	}

	

    void createOrange() {
		MeshBuilder builder;

        float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
        float sphere_diff[] = { sphere_color.x, sphere_color.y, sphere_color.z, 0.6f };
        float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        float shininess = 100.0f;
        int texcount = 2;
        vec3 sphere_pos = getPosition() +  vec3(0,radius,0);
        sphere = createSphere(radius, 100);
        sphere = builder.setMesh(sphere, amb, sphere_diff, spec, emissive, shininess, texcount, sphere_pos);
        float stalk_diff[] = { stalk_color.x, stalk_color.y, stalk_color.z, stalk_color.w };
        vec3 stalk_pos = sphere_pos + vec3(0, radius, 0);
		texcount = 0;
        stalk = createCylinder(radius/2, 0.1f, 20);
        stalk = builder.setMesh(stalk, amb, stalk_diff, spec, emissive, shininess, texcount, stalk_pos);
    }

	void sphereTransformations() {
		setIdentityMatrix(orange_transformations);
		translate(MODEL, getPosition().x, getPosition().y + radius, getPosition().z);
		float speed_scalar = getSpeed();
		vec3 speed = getSpeedVector(1.0f).normalize();
		vec3 res;
		res.x = speed.y * 0.0f - 1.0f * speed.z;
		res.y = speed.z * 0.0f - 0.0f * speed.x;
		res.z = speed.x * 1.0f - 0.0f * speed.y;
		rotate(MODEL, -own_axis_angle, res.x, res.y, res.z);
		multMatrix(orange_transformations, get(MODEL));
		translate(MODEL, 0, 0, 0);
	}

	void stalkTransformations() {
		multMatrix(MODEL, orange_transformations);
		translate(MODEL, 0, radius, 0);
	}

	void drawOrange(VSShaderLib shader, GLint pvm_uniformId, GLint vm_uniformId, GLint normal_uniformId, GLint lPos_uniformId) {
		MeshBuilder builder;
		//----------------Sphere-------------------
		builder.setShaders(shader, sphere);
		pushMatrix(MODEL);
		sphereTransformations();
		if (countdown == 0) {
			builder.drawMesh(sphere, shader);
		}
		popMatrix(MODEL);
		//-----------------------------------------
		
		//----------------Stalk--------------------
		setShaders(shader, stalk);
		pushMatrix(MODEL);
		stalkTransformations();
		if (countdown == 0) {
			builder.drawMesh(stalk, shader);
		}
		popMatrix(MODEL);
		//-----------------------------------------
	}

	AABB getBoundingBox() {
		return GameObject::getBoundingBox();
	}
};

#endif
