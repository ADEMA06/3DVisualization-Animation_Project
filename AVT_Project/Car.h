#ifndef __CAR_H__
#define __CAR_H__

#define _USE_MATH_DEFINES
#include "GameObject.h"
#include <sstream>
#include <string>
#include "geometry.h"
#include "AVTmathLib.h"
#include "VSShaderlib.h"
#include <math.h>
#include "Light.h"

const float car_width = 1.0f;
const float car_height = 0.5f;
const float car_thickness = 0.5f;

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

class Car : public GameObject {
    std::vector<struct MyMesh> tires;
    struct MyMesh body;
    float max_speed;
    vec4 body_color;
    vec4 tires_color;
	float accel = 0;
	float a = 0;

	Light spotlight;

public:
    Car(vec3 position, float accel, float max_speed, vec4 body_color, vec4 tires_color) : GameObject(position) {
        this->max_speed = max_speed;
        this->body_color = body_color;
        this->tires_color = tires_color;
		this->accel = accel;
		spotlight.direction = vec4(1.0f, 0.0f, 0.0f, 0.0f);
		spotlight.position = vec4(position.x + car_width / 2 + 0.1f, position.y + 0.3f, position.z, 1.0f);
		float angle = 15.0f * 3.1415f / 180.0f;
		spotlight.cut_off = cos(angle);
    }

	void goForward(float dt) {
		setSpeed(getSpeed() + accel * dt);
	}

	void goBackwards(float dt) {
		setSpeed(getSpeed() - accel * dt);
	}

	void goLeft(float dt) {
		setRotAngle(getRotAngle() + 50.0f * dt);
		float angle = getRotAngle() * M_PI / 180;
		spotlight.position = vec4(spotlight.position.x + ((car_width + 0.3f) / 2) * dt * sin(-angle), spotlight.position.y, spotlight.position.z + ((car_width + 0.3f) / 2) * dt * cos(M_PI-angle), 1.0f);
	}

	void goRight(float dt) {
		setRotAngle(getRotAngle() - 50.0f * dt);
		float angle = getRotAngle() * M_PI / 180;
		spotlight.position = vec4(spotlight.position.x + ((car_width + 0.3f) / 2) * dt * sin(angle), spotlight.position.y, spotlight.position.z + ((car_width + 0.3f) / 2) * dt * cos(angle), 1.0f);

	}

	void stop(float dt) {
		if(getSpeed() > 0)
			setSpeed(std::max(getSpeed() - accel*3 * dt, 0.0f));
		else
			setSpeed(std::min(getSpeed() + accel*3 * dt, 0.0f));
	}

	void update(float dt) {
		float speed = getSpeed();
		float angle = getRotAngle() * M_PI / 180;
		vec3 position = getPosition();
		vec3 speed_vector = vec3(speed * cos(angle) * dt, 0.0f, speed * sin(-angle) * dt);
		setPosition(position + speed_vector);
		int count = 0;
		for (int i = -1; i <= 1; i += 2) {
			for (int j = -1; j <= 1; j += 2) {
				vec3 position = getPosition() + vec3(car_width / 2 * i + 0.25f * (-i), 0.0f, car_height / 2 * j);
				tires.at(count).position = position;
				count++;
			}
		}
		spotlight.position = vec4(spotlight.position.x + speed_vector.x, spotlight.position.y + speed_vector.y, spotlight.position.z + speed_vector.z, 1.0f);
		spotlight.direction = vec4(cos(angle), 0, sin(-angle), 0.0f);
	}

	void createCar() {
		float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
		float diff[] = { body_color.x, body_color.y, body_color.z, body_color.w};
		float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 100.0f;
		int texcount = 0;
		vec3 car_pos = getPosition();
		body = createCube();
		body = setMesh(body, amb, diff, spec, emissive, shininess, texcount, car_pos);
		MyMesh amesh;
		float tires_diff[] = { tires_color.x, tires_color.y, tires_color.z, tires_color.w };
		for (int i = -1; i <= 1; i += 2) {
			for (int j = -1; j <= 1; j += 2) {
				amesh = createTorus(0.05f, 0.25f, 20, 50);
				vec3 position = car_pos + vec3(car_width/2*i + 0.25f*(-i), 0.0f, car_height/2*j);
				amesh = setMesh(amesh, amb, tires_diff, spec, emissive, shininess, texcount, position);
				tires.push_back(amesh);
			}
		}
	}

	void bodyTransformations() {
		translate(MODEL, getPosition().x, getPosition().y, getPosition().z);
		rotate(MODEL, getRotAngle(), 0.0f, 1.0f, 0.0f);
		translate(MODEL, -car_width / 2, 0.25, -car_height / 2);
		scale(MODEL, car_width, car_thickness, car_height);
	}

	void tireTransformations(int i) {
		vec3 position = getPosition();
		translate(MODEL, position.x, position.y,position.z);
		rotate(MODEL, getRotAngle(), 0.0f, 1.0f, 0.0f);
		translate(MODEL, tires.at(i).position.x-position.x, tires.at(i).position.y + 0.25f - position.y, tires.at(i).position.z - position.z);
		rotate(MODEL, 90.0f, 1.0f, 0.0f, 0.0f);
	}

	void drawCar(VSShaderLib shader, GLint pvm_uniformId, GLint vm_uniformId, GLint normal_uniformId, GLint lPos_uniformId) {
		setShaders(shader, body);
		float res[4];
		float res1[4];
		float res2[4];
		float lights_pos[4] = { spotlight.position.x, spotlight.position.y, spotlight.position.z, spotlight.position.w };
		float lights_dir[4] = { spotlight.direction.x, spotlight.direction.y, spotlight.direction.z, 0.0f };

		multMatrixPoint(VIEW, lights_dir, res2);
		GLint loc = glGetUniformLocation(shader.getProgramIndex(), "uni_spotlights.direction");
		glUniform4fv(loc, 1, res2);

		multMatrixPoint(VIEW, lights_pos, res);   //lightPos definido em World Coord so is converted to eye space
		loc = glGetUniformLocation(shader.getProgramIndex(), "uni_spotlights.position");
		glUniform4fv(loc, 1, res);

		loc = glGetUniformLocation(shader.getProgramIndex(), "uni_spotlights.on");
		glUniform1d(loc, true);

		loc = glGetUniformLocation(shader.getProgramIndex(), "uni_spotlights.cutOff");
		glUniform1f(loc, spotlight.cut_off);

		pushMatrix(MODEL);
		bodyTransformations();
		drawMesh(body, shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
		popMatrix(MODEL);
		for (int i = 0; i < tires.size(); i++) {
			setShaders(shader, tires.at(i));
			pushMatrix(MODEL);
			tireTransformations(i);
			drawMesh(tires.at(i), shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
			popMatrix(MODEL);
		}
	}

};


#endif
