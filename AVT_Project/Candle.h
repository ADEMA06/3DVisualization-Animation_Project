#ifndef __CANDLE_H__
#define __CANDLE_H__

#include "StaticObject.h"
#include <string>
#include "MeshBuilder.h"
#include "Light.h"
#include "vec.h"


const float radius = 0.5f;
const float height = 4.0f;

class Candle : public StaticObject{
	struct MyMesh candle;
	Light pointlight;
	vec4 color;

public:
	Candle(vec3 position, vec4 color) : StaticObject(position) {
		this->color = color;
		this->pointlight.position = vec4(position.x, position.y + height, position.z, 1.0f);
		this->pointlight.on = 1;
	}

	Light getPointLight() {
		return pointlight;
	}

	void createCandle() {
		MeshBuilder builder;
		float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
		float diff[] = { color.x, color.y, color.z, color.w };
		float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 10.0f;
		int texcount = 0;
		vec3 pos = getPosition();
		candle = createCylinder(height, radius, 20);
		candle = builder.setMesh(candle, amb, diff, spec, emissive, shininess, texcount, pos);
	}


	void drawCandle(VSShaderLib *shader) {
		MeshBuilder builder;
		builder.setShaders(shader, candle);
		pushMatrix(MODEL);
		translate(MODEL, getPosition().x, getPosition().y + height/2, getPosition().z);
		rotate(MODEL, 90, 0, 1, 0);
		GLint view_uniformId = glGetUniformLocation(shader->getProgramIndex(), "m_View");
		glUniformMatrix4fv(view_uniformId, 1, GL_FALSE, mMatrix[VIEW]);
		GLint texMode_uniformId = glGetUniformLocation(shader->getProgramIndex(), "texMode");
		glUniform1i(texMode_uniformId, 3);
		builder.drawMesh(candle, shader);
		popMatrix(MODEL);
		
	}
};

#endif