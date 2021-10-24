#ifndef __TABLE_H__
#define __TABLE_H__

#include <string>
#include "MeshBuilder.h"
#include "StaticObject.h"

class Table : public StaticObject {
	//Meshes
	struct MyMesh base;
	std::vector<struct MyMesh> legs;

	//Dimensions
	float width;
	float height;
	float thickness;
	float legSide;
	float legHeight;


public:

	Table(float width, float height, float thickness, float legSide, float legHeight, vec3 position) : StaticObject(position) {
		this->height = height;
		this->width = width;
		this->thickness = thickness;
		this->legSide = legSide;
		this->legHeight = legHeight;
	}

	void createTable() {
		MeshBuilder builder;

		float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
		float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
		float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 500.0f;
		int texcount = 3;
		vec3 table_pos = getPosition();
		base = createCube();
		base = builder.setMesh(base, amb, diff, spec, emissive, shininess, texcount, table_pos);
		MyMesh amesh;
		for (int i = -1; i <= 1; i+=2) {
			for(int j = -1; j <= 1; j+=2) {
				amesh = createCube();
				vec3 position = table_pos + vec3(j*width/2 - j*legSide/2, -legHeight, i * height / 2 - i * legSide/2);
				amesh = builder.setMesh(amesh, amb, diff, spec, emissive, shininess, texcount, position);
				legs.push_back(amesh);
			}
		}
	}

	void drawTable(VSShaderLib shader) {
		MeshBuilder builder;
		builder.setShaders(shader, base);
		pushMatrix(MODEL);
		translate(MODEL, -width/2, -thickness, -height/2);
		translate(MODEL, getPosition().x, getPosition().y, getPosition().z);
		scale(MODEL, width, thickness, height);
		builder.drawMesh(base, shader);
		popMatrix(MODEL);


		for (int i = 0; i < legs.size(); i++) {
			builder.setShaders(shader, legs.at(i));
			pushMatrix(MODEL);
			translate(MODEL, -legSide/2, -thickness, -legSide/2);
			translate(MODEL, legs.at(i).position.x, legs.at(i).position.y, legs.at(i).position.z);
			scale(MODEL, legSide, legHeight, legSide);
			builder.drawMesh(legs.at(i), shader);
			popMatrix(MODEL);
		}
	}
};

#endif