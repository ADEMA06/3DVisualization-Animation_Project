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

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

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
		printf("%f\n", getRotAngle());
		int offset[2] = { -1, 1 };
		/*if (getPosition().x > tablePos.x + tableWidth / 2 || getPosition().x < tablePos.x - tableWidth / 2 || getPosition().y > tablePos.y + tableHeight / 2 || getPosition().y < tablePos.y - tableHeight / 2) {
			int j = rand() % 2;
			vec3 position = vec3(offset[j] * rand() % ((int)tableWidth/2), 0.0f, offset[j] * (rand() % ((int)tableHeight/2)));
			setPosition(position);
		}*/
		
	}

	float rotationSpeed() {
		float speedIntensity = std::cbrt(std::pow(getSpeed().x, 3) + std::pow(getSpeed().y, 3) + std::pow(getSpeed().z, 3));
		return (speedIntensity / 2 * M_PI * radius);
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

	void drawOrange(VSShaderLib shader, GLint pvm_uniformId, GLint vm_uniformId, GLint normal_uniformId, GLint lPos_uniformId) {
		GLint loc;
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, sphere.mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, sphere.mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, sphere.mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, sphere.mat.shininess);
		pushMatrix(MODEL);
		translate(MODEL, getPosition().x, getPosition().y+radius, getPosition().z);
		rotate(MODEL, getRotAngle(), 0.0f, 0.0f, 1.0f);
		translate(MODEL, 0, 0, 0);
		
			
		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// Render mesh
		glBindVertexArray(sphere.vao);

		if (!shader.isProgramValid()) {
			printf("Program Not Valid!\n");
			exit(1);
		}
		glDrawElements(sphere.type, sphere.numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);

		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, stalk.mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, stalk.mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, stalk.mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, stalk.mat.shininess);
		pushMatrix(MODEL);
		translate(MODEL, getPosition().x, getPosition().y+radius, getPosition().z);
		rotate(MODEL, -getRotAngle(), 0.0f, 0.0f, 1.0f);
		translate(MODEL, 0, radius, 0);
		rotate(MODEL, dirRotation, 0.0f, 1.0f, 0.0f);
		

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// Render mesh
		glBindVertexArray(stalk.vao);

		if (!shader.isProgramValid()) {
			printf("Program Not Valid!\n");
			exit(1);
		}
		glDrawElements(stalk.type, stalk.numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);
	}
};

#endif
