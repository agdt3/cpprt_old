#include "src/camera.h"

Camera::Camera(mat4* ctw, float w, float h, float fx, float fy, vec3 ori) {
	cameraToWorld = *ctw; //start at origin
	worldToCamera = glm::inverse(cameraToWorld);
    origin = ori;
    fovx = fx;
    fovy = fy;
    width = w;
    height = h;
    aspect_ratio = w / h;
    angle = tan(Transform::degToRad(fovx/2));
}
