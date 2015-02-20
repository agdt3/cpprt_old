// Transform header file to define the interface. 
// The class is all static for simplicity
// You need to implement left, up and lookAt
// Rotate is a helper function

// Include the helper glm library, including matrix transform extensions

#ifndef TRANSFORM_H
#define TRANSFORM_H

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include "glm.hpp"
//#include "gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <ostream>

// glm provides vector, matrix classes like glsl
// Typedefs to make code more readable 

typedef glm::mat3 mat3 ;
typedef glm::mat4 mat4 ; 
typedef glm::vec3 vec3 ; 
typedef glm::vec4 vec4 ; 
//const float pi = 3.14159265 ; // For portability across platforms

class Transform  
{
public:
	Transform();
	virtual ~Transform();
	static void left(float degrees, vec3& eye, vec3& up);
	static void up(float degrees, vec3& eye, vec3& up);
	static mat4 lookAt(const vec3& eye, const vec3 &center, const vec3& up);
	static mat4 perspective(float fovy, float aspect, float zNear, float zFar);
    static mat3 rotate(const float degrees, const vec3& axis) ;
    static vec3 rotate_vec(const float degrees, const vec3& dir, const vec3& axis);        
    static vec3 reflect(const vec3 ivec, const vec3 nvec); 
    static mat4 scale(const float &sx, const float &sy, const float &sz) ; 
    static mat4 translate(const float &tx, const float &ty, const float &tz);
    static vec3 upvector(const vec3 &up, const vec3 &zvec) ; 
	static float degToRad(float degrees);
    static bool solve_quadratic(float a, float b, float c, float&, float&);
};

// toString representation of vec3
inline
std::ostream & operator<< (std::ostream &stream, vec3 const &v){
    stream << "x " << v.x << " y " << v.y << " z " << v.z << std::endl;
    return stream;
}

#endif
