#include "transform.h"
#include "variables.h"
#include <ostream>

#ifndef RAY_H
#define RAY_H

class Ray {
public:
	vec3 origin, direction;
	RayType type;

	Ray(vec3 orig, vec3 dir, RayType rt);
	vec3 operator() (const float &t);
};

// toString representation of ray
inline
std::ostream & operator<< (std::ostream &stream, Ray const &r){
    stream << "ray origin (" << r.origin << ") ray dir (" << r.direction << ")" << std::endl;
    return stream;
}

#endif
