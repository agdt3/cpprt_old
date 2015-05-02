#include "src/camera.h"

Camera::Camera(mat4* ctw, float w, float h, float fx, float fy, vec3 ori) {
	camera_to_world = *ctw; //start at origin
	world_to_camera = glm::inverse(camera_to_world);
    origin = ori;
    fovx = fx;
    fovy = fy;
    width = w;
    height = h;
    aspect_ratio = w / h;
    angle = tan(Transform::degToRad(fovx/2));
}

void Camera::set_fovx(float fx) {
    fovx = fx;
    angle = tan(Transform::degToRad(fovx/2));
}
