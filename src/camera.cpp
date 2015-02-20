#include "src/camera.h"

Camera::Camera(mat4* ctw) {
	fov = FOVX;
	angle = ANGLE;
	nearClippingPlane = NEAR;
	farClippingPlane = FAR;
	cameraToWorld = *ctw; //start at origin
	worldToCamera = glm::inverse(cameraToWorld);
    vec3 origin = vec3(0.0);
}
