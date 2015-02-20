#include "variables.h"

#ifndef CAMERA_H
#define CAMERA_H

class Camera {
public:
	float nearClippingPlane, farClippingPlane;
	float fov;
	float angle;
	mat4 cameraToWorld, worldToCamera;
    vec3 origin;

    Camera(mat4*);
};

#endif
