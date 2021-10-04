#ifndef __CAR_H__
#define __CAR_H__

#include "GameObject.h"
#include <sstream>
#include <string>
#include "geometry.h"
#include "AVTmathLib.h"
#include "VSShaderlib.h"

const float car_width = 1.0f;
const float car_height = 0.5f;
const float car_thickness = 0.5f;

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

class Car : GameObject {
    std::vector<struct MyMesh> tires;
    struct MyMesh body;
    float max_speed;
    vec4 body_color;
    vec4 tires_color;

public:
    Car(vec3 position, float max_speed, vec4 body_color, vec4 tires_color) : GameObject(position) {
        this->max_speed = max_speed;
        this->body_color = body_color;
        this->tires_color = tires_color;
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
		translate(MODEL, -car_width / 2, 0.25, -car_height / 2);
		translate(MODEL, getPosition().x, getPosition().y, getPosition().z);
		scale(MODEL, car_width, car_thickness, car_height);
	}

	void tireTransformations(int i) {
		translate(MODEL, tires.at(i).position.x, tires.at(i).position.y + 0.25f, tires.at(i).position.z);
		rotate(MODEL, 90.0f, 1.0f, 0.0f, 0.0f);
		scale(MODEL, 1.0f, 1.0f, 1.0f);
	}

	void drawCar(VSShaderLib shader, GLint pvm_uniformId, GLint vm_uniformId, GLint normal_uniformId, GLint lPos_uniformId) {
		setShaders(shader, body);
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
