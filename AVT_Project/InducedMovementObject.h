#ifndef __IND_MOV_OBJECT_H__
#define __IND_MOV_OBJECT_H__
#include "GameObject.h"

class InducedMovementObject : public GameObject {
	float accel;
	float speed_scalar;
	vec3 induced_speed;
	bool isPaused;
public:
	InducedMovementObject(vec3 position) : GameObject(position) {
		this->isPaused = false;
	};

	void setInducedSpeed(vec3 ind_speed) {
		induced_speed = ind_speed;
	}
    
    vec3 getInducedSpeed() {
        return induced_speed;
    }

	void setAccel(float a) {
		accel = a;
	}

	float getAccel() {
		return accel;
	}

	void setSpeedScalar(float s) {
		speed_scalar = s;
	}

	float getSpeedScalar() {
		return speed_scalar;
	}

	void setPause(bool pauseValue) {
		this->isPaused = pauseValue;
	}

	bool getPause() {
		return this->isPaused;
	}



	void collision_reaction(vec3 car_pos, float speed_scalar, float accel) {
		vec3 dir = (getPosition() - car_pos).normalize();
		this->speed_scalar = speed_scalar;
		setAccel(accel);
		setInducedSpeed(dir * this->speed_scalar);
	}

	void update(float dt) {
		if (isPaused) return;
		induced_speed = induced_speed + induced_speed.normalize() * accel * dt;
		setSpeedScalar(getSpeedScalar() + accel * dt);
		if (getSpeedScalar() > 0) {
			vec3 offset = induced_speed * dt;
			updateBoundingBox(offset);
			setPosition(getPosition() + offset);
		} 
		else {
			setSpeedScalar(0.0f);
			accel = 0;
			induced_speed = vec3(0.0f, 0.0f, 0.0f);
		}
	}

};

#endif