#include "variables.h"
#include "transform.h"

#ifndef CAMERA_H
#define CAMERA_H

class Camera {
public:
	float near = 1.0;
    float far = 10^8;
    int bpp = 24;
    float width, height, fovx, fovy, angle, aspect_ratio;
	mat4 cameraToWorld, worldToCamera;
    vec3 origin;

    Camera(mat4*, float, float, float, float, vec3);
};

// toString representation of camera
inline
std::ostream & operator<< (std::ostream &stream, Camera const &c){
    stream << "camera origin " << c.origin << " width " << c.width << " height " << c.height << std::endl;
    return stream;
}

#endif
