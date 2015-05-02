#include "transform.h"
#include "variables.h"
#include <ostream>

#ifndef RAY_H
#define RAY_H

class Ray
{
public:
    static int id_generator;
    int id;
    vec3 origin, direction;
	RayType type;

	Ray(vec3, vec3, RayType);
	Ray(vec3, vec3, RayType, int); //id constructor for testing
    Ray(const Ray&); //copy which sets new id
	vec3 operator() (const float &t);
};

// toString representation of ray
inline
std::ostream & operator<< (std::ostream &stream, Ray const &r){
    stream << "ray type " << (int)r.type << std::endl << "ray origin (" << r.origin << ")" << std::endl << "ray dir (" << r.direction << ")" << std::endl;
    return stream;
}
#endif
