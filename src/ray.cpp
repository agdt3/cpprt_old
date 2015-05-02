#include "src/ray.h"
#include <iostream>

int Ray::id_generator = 0;

Ray::Ray(vec3 orig, vec3 dir, RayType rt) {
    id = id_generator++;
	origin = orig;
	direction = dir;
	type = rt;
};

//used for testing
Ray::Ray(vec3 orig, vec3 dir, RayType rt, int new_id) {
    id = new_id;
	origin = orig;
	direction = dir;
	type = rt;
};

//copy
Ray::Ray(const Ray& source) {
    id = id_generator++;
	origin = source.origin;
	direction = source.direction;
	type = source.type;
};

//calling rayinst(t) returns a point on this ray at some distance t
vec3 Ray::operator() (const float &t) {
	return origin + direction * t;
};

