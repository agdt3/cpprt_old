// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.

#include "src/transform.h"

//my own helper
float Transform::degToRad(float degrees) {
	return degrees/57.295;
}

float radToDeg(float rad) {
	return rad * 57.295;
}

mat3 Transform::rotate(const float degrees, const vec3& axis) {
  //I guess we'll try to implement the rodrigues transform
  //added axis normalization
  //vec3 naxis = glm::normalize(axis);

  float rads = degToRad(degrees);
  mat3 I = mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
  mat3 result = mat3(0.0);
  mat3 astar = mat3(0, axis[2], -axis[1], -axis[2], 0, axis[0], axis[1], -axis[0], 0);
  mat3 outer = glm::outerProduct(axis, axis);
  //result = I*cos(rads) + glm::outerProduct(axis, axis) * (1-cos(rads)) + astar*sin(rads);
  result = I * (float)cos(rads) + outer * (float)(1-cos(rads)) + astar * (float)sin(rads);
  return result;
}

vec3 Transform::rotate_vec(const float degrees, const vec3& dir, const vec3& axis) {
    //rotate dir around axis by degrees
    float rads = degToRad(degrees);
    vec3 ax = glm::normalize(axis);
    vec3 res = (dir * (float)cos(rads)) + (glm::cross(ax, dir) * (float)sin(rads)) + (ax * (glm::dot(ax, dir)) * (float)(1.0 - cos(rads))); 
    return res;
}

vec3 Transform::reflect(const vec3 ivec, const vec3 nvec) {
    vec3 res = ivec - ((float)2.0 * (glm::dot(nvec, ivec) * nvec));
    return res;
}

// Transforms the camera left around the "crystal ball" interface
void Transform::left(float degrees, vec3& eye, vec3& up) {
  //the rotation vector should be the unit vector version of the up vector
  //vec3 axis = glm::normalize(up);
  //vec3 axis = upvector(up, eye); //normalize the up vector
  vec3 axis = up;
  mat3 rot = rotate(degrees, axis);
  eye = rot*eye; //3x3 by 3x1 = 3x1
  up = rot*up;
}

// Transforms the camera up around the "crystal ball" interface 
void Transform::up(float degrees, vec3& eye, vec3& up) {
  //the rotation vector should be a unit vector perpendicular to up and eye (forward)
  //i.e. if up is y, and forward is z
  //the roation is around x
  //but this changes depedning on the position of the up and eye vectors
  //normalized (i.e. made a unit vector) for rotation purposes
  //vec3 newup = upvector(up, eye);
  
  vec3 axis = glm::normalize(glm::cross(eye, up)); //normalized normal to up and eye
  //printf("UP axis %.2f, %.2f, %.2f\n", axis.x, axis.y, axis.z); 
  mat3 rot = rotate(degrees, axis);
  eye = rot*eye; //3x3 by 3x1 = 3x1
  up = rot*up;
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
{
  //eye vector is [0, 0, 5] by default?
  //center is [0] by default
  //up is [0, 1, 0] by default

  //a is eye, b is up
  //technically a should be eye-center, but I have no idea where center is
  vec3 a = eye - center;
  //vec3 w = glm::normalize(eye);
  vec3 w = glm::normalize(a);
  //vec3 u = glm::cross(up, w) / glm::abs(glm::cross(up, w));
  vec3 u = glm::normalize(glm::cross(up, w));
  vec3 v = glm::cross(w, u);

  mat4 r = mat4(u[0], v[0], w[0], 0, u[1], v[1], w[1], 0, u[2], v[2], w[2], 0, 0, 0, 0, 1);
  //mat4 t = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -eye[0], -eye[1], -eye[2], 1);
  mat4 t = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -eye[0], -eye[1], -eye[2], 1);
  mat4 rt = r*t;
  //mat4 rt = mat4(u[0], v[0], w[0], 0, u[1], v[1], w[1], 0, u[2], v[2], w[2], 0, glm::dot(-u, eye), glm::dot(-v, eye), glm::dot(-w, eye), 1); 
  // You will change this return call
  //printf("Coordinates: %.2f, %.2f, %.2f; distance: %.2f\n", eye.x, eye.y, eye.z, sqrt(pow(eye.x, 2) + pow(eye.y, 2) + pow(eye.z, 2)));
  return rt;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
	//works with radians!
	float theta = degToRad(fovy) / 2.0;
	float d = 1.0 / glm::tan(theta);

	float A = (-zFar + zNear) / (zFar - zNear);
	float B = (-2 * zFar * zNear) / (zFar - zNear);
	mat4 ret = mat4(d/aspect, 0, 0, 0, 0, d, 0, 0, 0, 0, A, -1, 0, 0, B, 0);

	//mat4 ret = mat4(1.0);
    return ret;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
	mat4 ret = mat4(sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1.0);
	// sx  0  0  0
	//  0 sy  0  0
	//  0  0 sz  0
	//  0  0  0 1.0

	//mat4 ret = mat4(1.0);
    return ret;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
	mat4 ret = mat4(1.0, 0, 0, 0, 0, 1.0, 0, 0, 0, 0, 1.0, 0, tx, ty, tz, 1.0);
    // Implement translation
	//  1.0  0  0  tx
	//  0  1.0  0  ty
	//  0  0  1.0  tz
	//  0  0  0  1.0
	//mat4 ret = mat4(1.0);
    return ret;
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
    vec3 x = glm::cross(up,zvec); 
    vec3 y = glm::cross(zvec,x); 
    vec3 ret = glm::normalize(y); 
    return ret; 
}

bool Transform::solve_quadratic(float a, float b, float c, float &r1, float &r2) {
    //ax^2 + bx + c = 0
    //x = (-b +/- sqrt(b^2 - 4ac)) / 2a
    if ((pow(b, 2) - (4.0 * a * c)) < 0) {
        //complex root - no intersection
        return false; 
    }    

    float formula = sqrt(pow(b, 2) - 4.0 * a * c); 
    r1 = (-b + formula) / (2 * a);
    r2 = (-b - formula) / (2 * a);
    
    //TODO: floats should be compared with some tolerance
    if (r1 == r2) {
        //equal is tangent
        return false;
    }
    else if (r1 > 0 && r2 > 0) {
        //two unequal, > 0 roots
        if (r2 < r1) {
            float temp = r1;
            r1 = r2;
            r2 = temp;
        }
        return true;
    }
    else if ((r1 > 0 && r2 < 0) || (r2 > 0 && r1 < 0)) {
        // one > 0, one < 0 - ray is inside sphere
        return false;
    }
    return false;
}

Transform::Transform()
{

}

Transform::~Transform()
{

}




