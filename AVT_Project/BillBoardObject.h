#ifndef __BILLBOARD_OBJECT_H__
#define __BILLBOARD_OBJECT_H__

#include "GameObject.h"
#include <sstream>
#include <string>
#include "geometry.h"
#include "AVTmathLib.h"
#include "VSShaderlib.h"
#include "StaticObject.h"
#include "MeshBuilder.h"
#include "I3DBillboard.h"


/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

extern GLint texMode_uniformId;

float tree_spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float tree_shininess = 5.0f;
int texcount = 0;

class BillBoardObject : public StaticObject {
	struct MyMesh quad;
	int texcount = 0;


public:
	BillBoardObject(vec3 position, int texcount) : StaticObject(position) {
		this->texcount = texcount;
	}


	void createBillBoard() {


		float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
		float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };

		float shininess = 100.0f;
		quad = createQuad(6, 6);
		memcpy(quad.mat.specular, tree_spec, 4 * sizeof(float));
		memcpy(quad.mat.emissive, emissive, 4 * sizeof(float));
		memcpy(quad.mat.ambient, amb, 4 * sizeof(float));
		memcpy(quad.mat.diffuse, diff, 4 * sizeof(float));
		quad.mat.shininess = tree_shininess;
		quad.mat.texCount = this->texcount;
	}

	void drawBillBoard(vec3 cam_pos, VSShaderLib* shader, int shadows) {

		if(!shadows)
			glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(texMode_uniformId, 1); // draw textured quads

		MeshBuilder builder;
		builder.setShaders(shader, quad);

		pushMatrix(MODEL);
		vec3 pos = getPosition();
		translate(MODEL, pos.x, pos.y, pos.z);

		float cam[3] = { cam_pos.x,cam_pos.y,cam_pos.z };
		float world_pos[3] = { pos.x, pos.y, pos.z };
		l3dBillboardCylindricalBegin(cam, world_pos);

		pushMatrix(MODEL);
		translate(MODEL, 0.0, 2.0, 0.0f);
		builder.drawMesh(quad, shader);
		popMatrix(MODEL);
		popMatrix(MODEL);
	}
};

#endif // !__BILLBOARD_OBJECT_H__