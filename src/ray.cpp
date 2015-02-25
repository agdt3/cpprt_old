#include "src/ray.h"
#include <iostream>

Ray::Ray(vec3 orig, vec3 dir, RayType rt) {
	origin = orig;
	direction = dir;
	type = rt;
};

//calling rayinst(t) returns a point on this ray at some distance t
vec3 Ray::operator() (const float &t) {
	return origin + direction * t;
};

