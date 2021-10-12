#ifndef __PERSPECTIVE_CAM_H__
#define __PERSPECTIVE_CAM_H__

#include "Camera.h"

class PerspectiveCamera : public Camera {
	float fov;
	vec3 spheric_coords;


public:
	PerspectiveCamera() {}

	PerspectiveCamera(vec3 position, vec3 target, bool fixed, float near_plane, float far_plane, float fov) : Camera(position, target, fixed, near_plane, far_plane) {
		this->fov = fov;
	}


	void setViewPort(int w, int h){
		float ratio;
		// Prevent a divide by zero, when window is too short
		if (h == 0)
			h = 1;
		// set the viewport to be the entire window
		glViewport(0, 0, w, h);
		// set the projection matrix
		ratio = (1.0f * w) / h;
		loadIdentity(PROJECTION);
		perspective(fov, ratio, getNearPlane(), getFarPlane());
	}

};

#endif