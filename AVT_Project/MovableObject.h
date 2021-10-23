#pragma once

#include <sstream>
#include <math.h>
#include "GameObject.h"

class MovableObject : public GameObject {
	float speed;
	float direction_angle;

public:
	MovableObject(vec3 position, float speed, float direction_angle = 0.0f) : GameObject(position) {
		this->speed = speed;
		this->direction_angle = direction_angle;
	}

	float getDirectionAngle() {
		return this->direction_angle;
	}

	void setDirectionAngle(float direction_angle) {
		this->direction_angle = direction_angle;
	}

	float getSpeed() {
		return this->speed;
	}

	void setSpeed(float speed) {
		this->speed = speed;
	}

	vec3 getSpeedVector(float dt) {
		float radians = this->direction_angle * M_PI / 180.0f;
		return vec3(speed * cos(radians) * dt, 0.0f, speed * sin(-radians) * dt);
	}
};
