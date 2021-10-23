#ifndef __CAR_H__
#define __CAR_H__

#define _USE_MATH_DEFINES
#include "MovableObject.h"
#include <string>
#include "MeshBuilder.h"
#include "Light.h"
#include "Camera.h"

const float car_width = 1.0f;
const float car_height = 0.5f;
const float car_thickness = 0.5f;

extern int current_camera;

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

class Car : public MovableObject {
	//Meshes and Mesh details
    std::vector<struct MyMesh> tires;
    struct MyMesh body;
	vec4 body_color;
	vec4 tires_color;

	//Movement details
    float max_speed;
	float accel = 0;

	//Lighs
	Light spotlight1;
	Light spotlight2;
	float spotlight_radius;

	//Matrix transformations
	float body_transformations[16];
	float cam_transformations[16];

	//Vector moved after update
	vec3 offset;

public:
    Car(vec3 position, float accel, float max_speed, vec4 body_color, vec4 tires_color) : MovableObject(position, 0.0f) {
		//Mesh details
		this->body_color = body_color;
		this->tires_color = tires_color;
        this->max_speed = max_speed;
		this->accel = accel;

		//Bounding box
		vec3 min_pos = vec3(-car_width / 2, -car_height / 2, -car_thickness / 2);
		vec3 max_pos = vec3(car_width / 2, car_height / 2, car_thickness / 2);
		setBoundingBox(min_pos, max_pos);

		//Spotlights
		spotlight1.on = 1;
		spotlight1.direction = vec4(1.0f, 0.0f, 0.0f, 0.0f);
		spotlight1.position = vec4(car_width / 2 + 0.05f, 0.15f, 0.0f, 1.0f);

		spotlight2.on = 1;
		spotlight2.direction = vec4(1.0f, 0.0f, 0.0f, 0.0f);
		spotlight2.position = vec4(car_width / 2 + 0.05f, 0.15f, car_width / 2, 1.0f);


		float angle = 15.0f * 3.1415f / 180.0f;
		spotlight1.cut_off = cos(angle);
		spotlight2.cut_off = cos(angle);

		spotlight_radius = car_width / 2 + 0.1f;

		//Transformation Matrices initialization
		setIdentityMatrix(body_transformations);
		setIdentityMatrix(cam_transformations);
    }

	float* getBodyTransformations() {
		return body_transformations;
	}

	void resetBoundingBox(){
		vec3 min_pos = vec3(- car_width / 2, - car_height / 2, - car_thickness / 2);
		vec3 max_pos = vec3(car_width / 2, car_height / 2, car_thickness / 2);
		setBoundingBox(min_pos, max_pos);
	}

	void goForward(float dt) {
		setSpeed(std::min(getSpeed() + accel * dt, max_speed));
	}

	void goBackwards(float dt) {
		setSpeed(std::max(getSpeed() - accel * dt, -max_speed));
	}

	void goLeft(float dt) {
		if(getSpeed() != 0)
			setDirectionAngle(getDirectionAngle() + 50.0f * dt);
	}

	void goRight(float dt) {
		if (getSpeed() != 0)
			setDirectionAngle(getDirectionAngle() - 50.0f * dt);
	}

	void stop(float dt) {
		if(getSpeed() > 0)
			setSpeed(std::max(getSpeed() - accel*3 * dt, 0.0f));
		else
			setSpeed(std::min(getSpeed() + accel*3 * dt, 0.0f));
	}

	void turnOnOffLights() {
		spotlight1.on = !spotlight1.on;
	}

	void update(float dt) {
		vec3 position = getPosition();
		vec3 speed_vector = getSpeedVector(dt);
		offset = speed_vector;
		setPosition(position + speed_vector);
		updateBoundingBox(body_transformations);
	}

	void createCar() {
		MeshBuilder builder;

		float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
		float diff[] = { body_color.x, body_color.y, body_color.z, 1.0};
		float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 100.0f;
		int texcount = 0;
		vec3 car_pos = getPosition();
		body = createCube();
		body = builder.setMesh(body, amb, diff, spec, emissive, shininess, texcount, car_pos);
		MyMesh amesh;
		float tires_diff[] = { tires_color.x, tires_color.y, tires_color.z, tires_color.w };
		for (int i = -1; i <= 1; i += 2) {
			for (int j = -1; j <= 1; j += 2) {
				amesh = createTorus(0.05f, 0.25f, 20, 50);
				vec3 position = vec3(car_width/2*i + 0.25f*(-i), 0.0f, car_height/2*j);
				amesh = builder.setMesh(amesh, amb, tires_diff, spec, emissive, shininess, texcount, position);
				tires.push_back(amesh);
			}
		}
	}

	bool checkCollision(AABB collision) {

		if (GameObject::checkCollision(collision)) {
			this->setPosition(this->getPosition() - offset);
			updateBoundingBox(vec3(offset.x*-1, offset.y*-1, offset.z*-1));
			this->setSpeed(0);
			return true;
		}else {
			return false;
		}
		
	}

	void bodyTransformations() {
		setIdentityMatrix(body_transformations);
		setIdentityMatrix(cam_transformations);
		translate(MODEL, getPosition().x, getPosition().y, getPosition().z);
		rotate(MODEL, getDirectionAngle(), 0.0f, 1.0f, 0.0f);
		multMatrix(body_transformations, get(MODEL));
		translate(MODEL, -car_width / 2, 0.25, -car_height / 2);
		multMatrix(cam_transformations, get(MODEL));
		scale(MODEL, car_width, car_thickness, car_height);
	}

	void tireTransformations(int i) {
		vec3 position = getPosition();
		multMatrix(MODEL, body_transformations);
		
		translate(MODEL, tires.at(i).position.x, tires.at(i).position.y + 0.25f, tires.at(i).position.z);
		rotate(MODEL, 90.0f, 1.0f, 0.0f, 0.0f);
	}

	void setCarLightsAndCamera(Camera* camera, VSShaderLib shader) {
		float res[4];
		float res1[4];

		float lights_pos1[4] = { spotlight1.position.x, spotlight1.position.y, spotlight1.position.z, spotlight1.position.w };
		float lights_dir1[4] = { spotlight1.direction.x, spotlight1.direction.y, spotlight1.direction.z, 0.0f };
		float lights_pos2[4] = { spotlight2.position.x, spotlight2.position.y, spotlight2.position.z, spotlight2.position.w };
		float lights_dir2[4] = { spotlight2.direction.x, spotlight2.direction.y, spotlight2.direction.z, 0.0f };

		pushMatrix(MODEL);

		//Calculate matrices used in car transformations
		bodyTransformations();

		//Up vector for lookat matrix calculation
		vec3 up(0, 1, 0);
		camera->setTransformations(cam_transformations);
		if (current_camera == 2) {
			camera->lookAtPoint({ getPosition().x, getPosition().y, getPosition().z }, up);
		}

		//Obtain spotlight positions and directions in eye coordinates--------------------------------------
		multMatrixPoint(cam_transformations, lights_dir1, res);
		multMatrixPoint(VIEW, res, res1);
		GLint loc = glGetUniformLocation(shader.getProgramIndex(), "uni_spotlights[0].direction");
		glUniform4fv(loc, 1, res1);

		multMatrixPoint(cam_transformations, lights_pos1, res);
		multMatrixPoint(VIEW, res, res1);   //lightPos definido em World Coord so is converted to eye space
		loc = glGetUniformLocation(shader.getProgramIndex(), "uni_spotlights[0].position");
		glUniform4fv(loc, 1, res1);

		loc = glGetUniformLocation(shader.getProgramIndex(), "spot_on");
		glUniform1i(loc, spotlight1.on);

		loc = glGetUniformLocation(shader.getProgramIndex(), "uni_spotlights[0].cutOff");
		glUniform1f(loc, spotlight1.cut_off);

		multMatrixPoint(cam_transformations, lights_dir2, res);
		multMatrixPoint(VIEW, res, res1);
		loc = glGetUniformLocation(shader.getProgramIndex(), "uni_spotlights[1].direction");
		glUniform4fv(loc, 1, res1);

		multMatrixPoint(cam_transformations, lights_pos2, res);
		multMatrixPoint(VIEW, res, res1);   //lightPos definido em World Coord so is converted to eye space
		loc = glGetUniformLocation(shader.getProgramIndex(), "uni_spotlights[1].position");
		glUniform4fv(loc, 1, res1);

		loc = glGetUniformLocation(shader.getProgramIndex(), "uni_spotlights[1].cutOff");
		glUniform1f(loc, spotlight1.cut_off);
		//---------------------------------------------------------------------------------------------------
	}

	void drawCar(VSShaderLib shader, Camera* camera) {
		MeshBuilder builder;
		builder.setShaders(shader, body);

		//Preset Light and Camera information (view and prespective matrices)
		setCarLightsAndCamera(camera, shader);

		builder.drawMesh(body, shader);
		popMatrix(MODEL);
		for (int i = 0; i < tires.size(); i++) {
			builder.setShaders(shader, tires.at(i));
			pushMatrix(MODEL);
			tireTransformations(i);
			builder.drawMesh(tires.at(i), shader);
			popMatrix(MODEL);
		}
	}

};


#endif
