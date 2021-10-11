#ifndef __CAM_H__
#define __CAM_H__

#include "vec.h"
#include "AVTmathLib.h"

class Camera {
	vec3 position;
	vec3 target;
	bool fixed;
	float far_plane;
	float near_plane;
	vec3 spheric_coords;
	vec3 final_position;

	
protected:
	Camera();
	Camera(vec3 position, vec3 target, bool fixed, float near_plane, float far_plane) : position(position), target(target), fixed(fixed), near_plane(near_plane), far_plane(far_plane) {};
public:
	vec3 getPosition() {
		return position;
	}

	void setPosition(vec3 position) {
		if (!fixed) {
			this->position = position;
		}
	}

	vec3 getFinalPosition() {
		return final_position;
	}

	void setFinalPosition(vec3 position) {
		if (!fixed) {
			this->final_position = position;
		}
	}

	void setTarget(vec3 target) {
		this->target = target;
	}

	void lookAtPoint(vec3 target, vec3 up) {
		setTarget(target);
		lookAt(final_position.x, final_position.y, final_position.z, target.x, target.y, target.z, up.x, up.y, up.z);
	}

	float getNearPlane() {
		return near_plane;
	}

	float getFarPlane() {
		return far_plane;
	}

	vec3 getSphericCoords() {
		return spheric_coords;
	}

	void setSphericCoords(vec3 spheric_coords) {
		this->spheric_coords = spheric_coords;
	}

	virtual void setViewPort(int w, int h) = 0;
};

#endif