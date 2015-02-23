#include "transform.h"

#ifndef VARIABLES_H
#define VARIABLES_H

//const int NUM_OBJECTS = 2;
const float MPI = 3.14125953589793;
/*
const int WIDTH = 640;
const float INVWIDTH = 1.0 / WIDTH;
const int HEIGHT = 480;
const float INVHEIGHT = 1.0 / HEIGHT;
const int BPP = 24;
const float ASPECT_RATIO = (float) WIDTH / (float) HEIGHT;
// a normal 4:3 perspective at 640x480 can handle roughly 30-45 degrees
// Widescreen resolutions and sizes can handle more
const float FOVX = 45.0; //fov should be 30-45 unless monitor is large and widescreen
//const float FOVY = FOVX / ASPECT_RATIO;
const float FOVY = 90.0;
const float ANGLE = tan(Transform::degToRad(FOVX/2));
//const float ANGLE = tan(MPI * 0.5 * FOVX / 180.0);
//const float INFINITY = pow(10.0, 8.0);
const float NEAR = 0.1;
const float FAR = 1000;
*/

enum class ObjType {
    sphere=0,
    plane,
    triangle,
    box,
    light
};

enum class LightType {
    ambient=0,
    point, //acts as a sphere
    directional
};

enum class RayType {
    general=0,
    camera,
    shadow
};

#endif
