#ifndef __BUTTER_H__
#define __BUTTER_H__

#include <sstream>
#include <string>
#include "MeshBuilder.h"

#include "InducedMovementObject.h"

const float butter_width = 2.0f;
const float butter_height = 0.75f;
const float butter_thickness = 0.75f;


class Butter : public InducedMovementObject {
    struct MyMesh butter_body;
    struct MyMesh butter_foil;
    vec4 foil_color;



public:
    Butter(vec3 position, vec4 foil_color) : InducedMovementObject(position) {
        this->foil_color = foil_color;
		vec3 min_pos = vec3(getPosition().x - butter_width / 2, getPosition().y - butter_height / 2, getPosition().z - butter_thickness / 2);
		vec3 max_pos = vec3(getPosition().x + butter_width / 2, getPosition().y + butter_height / 2, getPosition().z + butter_thickness / 2);
		setBoundingBox(min_pos, max_pos);
		setAccel(0.0f);

	}

	void createButter() {
		MeshBuilder builder;
		float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
		float foil_diff[] = { foil_color.x, foil_color.y, foil_color.z, 0.5f};
		float butter_diff[] = { 0.92f, 0.90f, 0.16f, 0.5f };
		float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 100.0f;
		int texcount = 0;
		vec3 butter_pos = getPosition();
		butter_body = createCube();
		butter_body = builder.setMesh(butter_body, amb, butter_diff, spec, emissive, shininess, texcount, butter_pos);
		butter_foil = createCube();
		butter_foil = builder.setMesh(butter_foil, amb, foil_diff, spec, emissive, shininess, texcount, butter_pos);
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

	void drawButter(VSShaderLib shader) {
		MeshBuilder builder;
		builder.setShaders(shader, butter_body);
		pushMatrix(MODEL);
		butterBodyTransformations();
		builder.drawMesh(butter_body, shader);
		popMatrix(MODEL);


		builder.setShaders(shader, butter_foil);
		pushMatrix(MODEL);
		butterFoilTransformations();
		builder.drawMesh(butter_foil, shader);
		popMatrix(MODEL);
	}


	AABB getBoundingBox() {
		return GameObject::getBoundingBox();
	}
};

#endif
