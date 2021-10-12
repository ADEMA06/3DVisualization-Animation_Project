#ifndef __CAM_H__
#define __CAM_H__

#include "vec.h"
#include "AVTmathLib.h"

class Camera {
	vec3 position;
	vec3 target;
	vec3 spheric_coord;
	bool fixed;
	float far_plane;
	float near_plane;
	vec3 spheric_coords;
	vec3 final_position;

	
protected:
	Camera() {
		spheric_coord = vec3(-4.753017f, 1.544328f, 0.154527f);
	}
	Camera(vec3 position, vec3 target, bool fixed, float near_plane, float far_plane) : position(position), target(target), fixed(fixed), near_plane(near_plane), far_plane(far_plane) {
		spheric_coord = vec3(-4.753017f, 1.544328f, 0.154527f);
	}
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

	void setTransformations(float* transformations) {
		float res[4];
		float pos[4] = { spheric_coord.x, spheric_coord.y, spheric_coord.z, 1.0f };
		multMatrixPoint(transformations, pos, res);
		position = vec3(res[0], res[1], res[2]);
	
	}

	void setSphericCoords(vec3 spheric_coords) {
		this->spheric_coord = spheric_coords;
	}

	virtual void setViewPort(int w, int h) = 0;
};

#endif