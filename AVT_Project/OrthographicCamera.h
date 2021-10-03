#ifndef __ORTOGRAPHIC_CAM_H__
#define __ORTOGRAPHIC_CAM_H__

#include "Camera.h"

class OrtographicCamera : public Camera {
	float left_plane;
	float right_plane;
	float top_plane;
	float bottom_plane;

public:
	OrtographicCamera(vec3 position, vec3 target, bool fixed, float far_plane, float near_plane, float left_plane, float right_plane, float top_plane, float bottom_plane) : Camera(position, target, fixed, far_plane, near_plane) {
		this->left_plane = left_plane;
		this->right_plane = right_plane;
		this->top_plane = top_plane;
		this->bottom_plane = bottom_plane;

	}

	void setViewPort(int w, int h) {
		// Prevent a divide by zero, when window is too short
		if (h == 0)
			h = 1;
		// set the viewport to be the entire window
		glViewport(0, 0, w, h);
		// set the projection matrix
		loadIdentity(PROJECTION);

		ortho(left_plane, right_plane, bottom_plane, top_plane, getNearPlane(), getFarPlane());
	}

};

#endif
