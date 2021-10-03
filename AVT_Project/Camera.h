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

	void setTarget(vec3 target) {
		this->target = target;
	}

	void lookAtPoint(vec3 target, vec3 up) {
		setTarget(target);
		lookAt(position.x, position.y, position.z, target.x, target.y, target.z, up.x, up.y, up.z);
	}

	float getNearPlane() {
		return near_plane;
	}

	float getFarPlane() {
		return far_plane;
	}

	virtual void setViewPort(int w, int h) = 0;
};

#endif