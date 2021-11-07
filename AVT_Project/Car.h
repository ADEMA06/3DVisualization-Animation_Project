#ifndef __CAR_H__
#define __CAR_H__

#define _USE_MATH_DEFINES
#include "MovableObject.h"
#include <string>
#include <fstream>
#include "AssimpMesh.h"
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

vec3 min_aabb;
vec3 max_aabb;

char model_dir[50] = "lowPolyCharger";

class Car : public MovableObject {
	//Meshes and Mesh details
    std::vector<struct MyMesh> meshes;
	vec4 body_color;
	vec4 tires_color;

	const aiScene* carScene = NULL;

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

	int lives = 100;
	float points = 0;

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

	int getLives() {
		return this->lives;
	}

	//TODO: Change points using speed instead of accel
	int getPoints() {
		return this->points;
	}

	void setPoints(int points) {
		if (points < 0) this->points = 0;
		else this->points = points;
	}

	void decrementLives() {
		this->lives -= 1;
	}

	float* getBodyTransformations() {
		return body_transformations;
	}

	void resetBoundingBox(){
		vec3 min_pos = vec3(- car_width / 2, - car_height / 2, - car_thickness / 2);
		vec3 max_pos = vec3(car_width / 2, car_height / 2, car_thickness / 2);
		setBoundingBox(min_aabb, max_aabb);
		setBoundingBoxDebug();
	}

	void goForward(float dt) {
		setSpeed(std::min(getSpeed() + accel * dt, max_speed));
		if(getSpeed() > 0) this->points += (getSpeed() * dt);
	}

	void goBackwards(float dt) {
		setSpeed(std::max(getSpeed() - accel * dt, -max_speed));
		if(this->points > 0) this->points += (getSpeed() * dt);

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
		if(getSpeed() > 0) setSpeed(std::max(getSpeed() - accel*3 * dt, 0.0f));
		else setSpeed(std::min(getSpeed() + accel*3 * dt, 0.0f));
		if(this->points > 0) this->points += (getSpeed() * dt);

	}

	void turnOnOffLights() {
		spotlight1.on = !spotlight1.on;
	}

	void update(float dt) {
		if (getPause()) return;

		vec3 position = getPosition();
		vec3 speed_vector = getSpeedVector(dt);
		offset = speed_vector;
		setPosition(position + speed_vector);
		updateBoundingBox(body_transformations);
	}

	void createCar(GLuint *textures, int offset) {
		MeshBuilder builder;
		strcpy(model_dir, "lowPolyCharger");
		std::string filepath;
		while (true) {

			std::ostringstream oss;
			oss << model_dir << "/" << model_dir << ".obj";;
			filepath = oss.str();   //path of OBJ file in the VS project

			strcat(model_dir, "/");
			//check if file exists
			std::ifstream fin(filepath.c_str());
			if (!fin.fail()) {
				fin.close();
				break;
			}
			else
				printf("Couldn't open file: %s\n", filepath.c_str());
		}
		//creation of Mymesh array with VAO Geometry and Material
		carScene = Import3DFromFile(filepath, carScene, &max_aabb, &min_aabb);
		if (!carScene)
			return;
		setBoundingBox(min_aabb, max_aabb);
		meshes = createMeshFromAssimp(carScene, textures, offset);

		for (int i = 0; i < meshes.size(); i++) {
			meshes[i].mat.texCount = 4;
		}

		float max_x = max_aabb.x*0.4f;
		float max_z = max_aabb.z*0.4f;
		float min_z = min_aabb.z*0.4f;

		spotlight1.position = vec4(max_x, 0.15f, min_z+0.1f, 1.0f);
		spotlight2.position = vec4(max_x, 0.15f, max_z-0.1f, 1.0f);
		setBoundingBoxDebug();
	}

	bool checkCollision(AABB collision) {

		if (GameObject::checkCollision(collision) && !getPause()) {
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
		multMatrix(cam_transformations, get(MODEL));
		rotate(MODEL, 180.0f, 0.0f, 1.0f, 0.0f);
		scale(MODEL, 0.4f, 0.4f, 0.4f);
		multMatrix(body_transformations, get(MODEL));
	}

	void setCarLightsAndCamera(Camera* camera, VSShaderLib *shader, bool repeated, Camera* rearview) {
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
		if ((current_camera == 2 || current_camera == 3) && !repeated) {
			camera->lookAtPoint({ getPosition().x, getPosition().y+0.3f, getPosition().z }, up);
		}
		if (repeated) {
		    rearview->setTransformations(cam_transformations);
			rearview->lookAtPoint({ getPosition().x, getPosition().y , getPosition().z }, up);
		}
	
		//Obtain spotlight positions and directions in eye coordinates--------------------------------------
		multMatrixPoint(cam_transformations, lights_dir1, res);
		multMatrixPoint(VIEW, res, res1);
		GLint loc = glGetUniformLocation(shader->getProgramIndex(), "uni_spotlights[0].direction");
		glUniform4fv(loc, 1, res1);

		multMatrixPoint(cam_transformations, lights_pos1, res);
		multMatrixPoint(VIEW, res, res1);   //lightPos definido em World Coord so is converted to eye space
		loc = glGetUniformLocation(shader->getProgramIndex(), "uni_spotlights[0].position");
		glUniform4fv(loc, 1, res1);

		loc = glGetUniformLocation(shader->getProgramIndex(), "spot_on");
		glUniform1i(loc, spotlight1.on);

		loc = glGetUniformLocation(shader->getProgramIndex(), "uni_spotlights[0].cutOff");
		glUniform1f(loc, spotlight1.cut_off);

		multMatrixPoint(cam_transformations, lights_dir2, res);
		multMatrixPoint(VIEW, res, res1);
		loc = glGetUniformLocation(shader->getProgramIndex(), "uni_spotlights[1].direction");
		glUniform4fv(loc, 1, res1);

		multMatrixPoint(cam_transformations, lights_pos2, res);
		multMatrixPoint(VIEW, res, res1);   //lightPos definido em World Coord so is converted to eye space
		loc = glGetUniformLocation(shader->getProgramIndex(), "uni_spotlights[1].position");
		glUniform4fv(loc, 1, res1);

		loc = glGetUniformLocation(shader->getProgramIndex(), "uni_spotlights[1].cutOff");
		glUniform1f(loc, spotlight1.cut_off);
		//---------------------------------------------------------------------------------------------------
	}

	void carRecursiveDraw(const aiScene* scene, aiNode* nd, VSShaderLib* shader, int offset, GLuint *textures) {
		MeshBuilder builder;
		GLint diffMapCount_loc = glGetUniformLocation(shader->getProgramIndex(), "diffMapCount");

		for (unsigned int n = 0; n < meshes.size(); ++n) {
			int diffMapCount = 0;
			glUniform1ui(diffMapCount_loc, 0);

			builder.setShaders(shader, meshes[n]);
			if (meshes[n].mat.texCount != 0) {
				for (unsigned int i = 0; i < meshes[n].mat.texCount; ++i) {
					if (meshes[n].texTypes[i] == DIFFUSE) {
						if (diffMapCount == 0) {
							diffMapCount++;
							glActiveTexture(GL_TEXTURE0 + offset);
							glBindTexture(GL_TEXTURE_2D, textures[offset]);
							GLint loc = glGetUniformLocation(shader->getProgramIndex(), "texUnitDiff");
							glUniform1i(loc, meshes[n].texUnits[i]);
							glUniform1i(diffMapCount_loc, diffMapCount);
						}
					}
				}
			}
			if (n != meshes.size() - 1) {
				builder.drawMesh(meshes[n], shader);
			}
		}

	}

	float* getCamTransformations() {
		return cam_transformations;
	}

	void drawRearView(VSShaderLib* shader, Camera* camera, int offset, GLuint* textures, bool repeated, Camera* rearview) {
		MeshBuilder builder;
		//Preset Light and Camera information (view and prespective matrices)
		setCarLightsAndCamera(camera, shader, repeated, rearview);
		GLint diffMapCount_loc = glGetUniformLocation(shader->getProgramIndex(), "diffMapCount");

		if (!repeated) {
			glStencilFunc(GL_NEVER, 1, 0x1);
			glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
			meshes[meshes.size() - 1].mat.diffuse[3] = 1.0f;
			pushMatrix(MODEL);
			meshes[meshes.size() - 1].mat.diffuse[3] = 1.0f;
			translate(MODEL, 0.0f, 0.23f, 0.0f);
			scale(MODEL, 1.0f, 0.8f, 1.0f);
			builder.drawMesh(meshes[meshes.size() - 1], shader);
			popMatrix(MODEL);
			glStencilFunc(GL_EQUAL, 0, 0x1);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			pushMatrix(MODEL);
			meshes[meshes.size() - 1].mat.diffuse[3] = 1.0f;
			scale(MODEL, 1.0f, 1.0f, 1.2f);
			builder.drawMesh(meshes[meshes.size() - 1], shader);
			popMatrix(MODEL);
		}
		glDisable(GL_CULL_FACE);
		carRecursiveDraw(carScene, carScene->mRootNode, shader, offset, textures);
		glEnable(GL_CULL_FACE);
		popMatrix(MODEL);
		updateBoundingBox(body_transformations);
	}

	void drawBoundingBox(VSShaderLib* shader) {
		getBoundingBox().draw(shader, cam_transformations);
	}

};


#endif
