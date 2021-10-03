#ifndef __ORANGE_H__
#define __ORANGE_H__

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

class Orange : GameObject {
    struct MyMesh stalk;
    struct MyMesh sphere;
    vec4 sphere_color;
    vec4 stalk_color;
    int speed_level;
    float radius;
	bool is_moving;

public:
    Orange(vec3 position, vec4 sphere_color, vec4 stalk_color, float radius, float speed) : GameObject(position, speed) {
        this->speed_level = 0;
        this->sphere_color = sphere_color;
        this->stalk_color = stalk_color;
        this->radius = radius;
		this->is_moving = true;
    }

    void createOrange() {
        float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
        float sphere_diff[] = { sphere_color.x, sphere_color.y, sphere_color.z, sphere_color.w };
        float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        float shininess = 100.0f;
        int texcount = 0;
        vec3 sphere_pos = getPosition() +  vec3(0,radius,0);
        sphere = createSphere(radius, 20);
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
		sphere.position = sphere.position + getSpeed();
		sphere.rotation.x -= sphere.rotAngle;
		if (sphere.rotation.x == -360)
			sphere.rotation.x = 0;
		translate(MODEL, sphere.position.x, sphere.position.y, sphere.position.z);
		rotate(MODEL, sphere.rotation.x, 0.0f, 0.0f, -1.0f);
		
			
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
		stalk.position = stalk.position + getSpeed();
		translate(MODEL, stalk.position.x, stalk.position.y-radius, stalk.position.z);
		rotate(MODEL, sphere.rotation.x, 0.0f, 0.0f, 1.0f);
		translate(MODEL, 0, radius, 0);
		

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
