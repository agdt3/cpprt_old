#include "src/objects.h"
#include "src/variables.h"
#include "src/transform.h"
#include <iostream>

Object::Object() {
	objectToWorld = mat4(1.0);
	worldToObject = glm::inverse(objectToWorld);
	color = vec4(0.0, 0.0, 0.0, 1.0);
}

Object::Object(mat4 *otw, vec4 col, float ease_dist=1.0) {
    objectToWorld = *otw;
    worldToObject = glm::inverse(objectToWorld);
    color = col;
    easing_distance = ease_dist;
}

//Uses Object constructor by passing it parameters //superclass constructor executes first
Sphere::Sphere(float r, mat4 *otw, vec4 col, float ease_dist) : Object(otw, col, ease_dist) {
	type = ObjType::sphere;
	center =  objectToWorld * vec4(0.0, 0.0, 0.0, 1.0);
    //std::cout << "s" << center.x << " " << center.y << " " << center.z << std::endl;
	radius = r; //define the radius explicitely, not as part of a transform, although really should
}

bool Sphere::intersects (const Ray &ray, vec3 &hit, vec3 &n, float &t0, float &t1) {
    int method = 2;
    if (method == 1) {
        vec3 center3 = vec3(center.x, center.y, center.z); //ignore w here for now
	    vec3 L = center3 - ray.origin;

        float t_ca = glm::dot(L, ray.direction);
        if(t_ca < 0) return false;

	    float dsqr = glm::dot(L, L) - pow(t_ca, 2);
        if(dsqr > (radius*radius)) return false;

	    float t_hc = sqrt(pow(radius, 2) - dsqr);
        t0 = t_ca - t_hc; //distance to point of impact
	    t1 = t_ca + t_hc; //distance to other side of sphere

        float dist = easing_distance * t0;
	    hit = ray.origin + (dist * ray.direction);

	    //try to calculate the n vector
	    //*n = (*hit - center3);
	    n = (hit - center3) / radius;

	    return true;
    }
    else if (method == 2) {
        vec3 center3 = vec3(center.x, center.y, center.z); //ignore w here for now
        vec3 L = ray.origin - center3;
        float a = glm::dot(ray.direction, ray.direction);
        float b = (float)2.0 * glm::dot(ray.direction, L);
        float c = glm::dot(L, L) - pow(radius, 2);

        float r1;
        float r2;
        bool hits = Transform::solve_quadratic(a, b, c, r1, r2);
        if (hits) {
            //std::cout << "hit" << std::endl;
            t0 = r1;
            t1 = r2;

            float dist = easing_distance * glm::abs(r1);
	        hit = ray.origin + (dist * (ray.direction - ray.origin));
	        n = (hit - center3) / radius;

            return true;
        }
        else {
            return false;
        }
    }
    return false;
}

Light::Light(float r, mat4 *otw, vec4 col, float ease_dist=1.0, LightType l_type=LightType::point) : Object(otw, col, ease_dist) {
    type = ObjType::light;
    ltype = l_type;
	center = objectToWorld * vec4(0.0, 0.0, 0.0, 1.0);
    direction = vec3(0.0);
	radius = r;
}

bool Light::intersects (const Ray &ray, vec3 &hit, vec3 &n, float &t0, float &t1) {
	//We want to know first if an intersection can even occur with this light
	//If it can't, then there's no reason to check the rest of the object stack

	//if (ray->type != RayType::shadow) {
	//	return false;
	//}

    //This is treating the point light like a sphere for now
    bool easing = true;
    if (ltype == LightType::ambient) {
		//ambient light is reachable from anywhere, except from behind an object
		return true;
	}
    else if (ltype == LightType::point) {
		//treat this like a sphere
		vec3 center3 = vec3(center.x, center.y, center.z); //ignore w here for now
        vec3 L = ray.origin - center3;

        float a = glm::dot(ray.direction, ray.direction);
        float b = (float)2.0 * glm::dot(ray.direction, L);
        float c = glm::dot(L, L) - pow(radius, 2);

        float r1;
        float r2;
        bool hits = Transform::solve_quadratic(a, b, c, r1, r2);
        if (hits) {
            //std::cout << "hit" << std::endl;
            t0 = r1;
            t1 = r2;

            float dist = easing_distance * glm::abs(r1);
            hit = ray.origin + (dist * ray.direction);

            n = (hit - center3) / radius;

            return true;
        }
        else {
            return false;
        }
    }
	else return false;
}

Triangle::Triangle(vec3 A, vec3 B, vec3 C, vec4 col) {
	v0 = A;
	v1 = B;
	v2 = C;
	n = glm::cross((v1 - v0), (v2 - v0));
	color = col;
	type = ObjType::triangle;
}

bool Triangle::intersects (const Ray &ray, vec3 &hit, vec3 &n_vec, float &t0, float &t1) {
	//first test if ray intersects the plane in whicht the triangle lives
	vec3 p0 = ray.origin;
	vec3 p1 = ray.direction;
	float denominator = glm::dot(n, p1 - p0);
	if (denominator == 0) {
		return false; //ray is in plane or parallel to plane
	}
	float nominator = glm::dot(n, v0 - p0);
	float dist = nominator / denominator;
	if (dist < 0) {
		return  false; //failed to intersect
	}

	//Now need to find if intersection point is in triangle (that's in the plane)
	vec3 point_at_dist = p0 + dist * (p1 - p0);

	//Point = v0 + s(v1-v0) + t(v2-v0) find s and t
	//point exists if s>=0; t>=0; s+t<=1;
	vec3 w  = point_at_dist - v0;
	vec3 u = v1 - v0;
	vec3 v = v2 - v0;

	float s_num = (glm::dot(u, v) * glm::dot(w, v)) - (glm::dot(v, v) * glm::dot(w, u));
	float t_num = (glm::dot(u, v) * glm::dot(w, u)) - (glm::dot(u, u) * glm::dot(w, v));
	float st_denom = (glm::dot(u, v) * glm::dot(u, v)) - (glm::dot(u, u) * glm::dot(v, v));
	float s = s_num / st_denom;
	float t = t_num / st_denom;

	if (s >= 0 && t >= 0 && s+t <= 1){
		t0 = dist;
		float eased_dist = glm::abs(dist) * 0.99;
		hit = p0 + eased_dist * (p1 - p0);
        n_vec = n;
		return true;
	}

	return false;
};

Plane::Plane(vec3 A, vec3 B, vec3 C, vec4 col){
    // these are more like 3 points in a plane you want to define
    // since any 3 points define a plane!
    v0 = A;
    v1 = B;
    v2 = C;
    n = glm::cross((v1 - v0), (v2 - v0));
    std::cout << "n x " << n.x << " y " << n.y << " z " << n.z << std::endl;
	color = col;
	type = ObjType::plane;
}

bool Plane::intersects (const Ray &ray, vec3 &hit, vec3 &n_vec, float &t0, float &t1) {
	//first test if ray intersects the plane in whicht the triangle lives
	vec3 p1 = ray.origin;
	vec3 p0 = ray.direction;

	float denominator = glm::dot(n, p1 - p0);
	if (denominator == 0) {
		return false; //ray is in plane or parallel to plane
	}
	float nominator = glm::dot(n, v0 - p0);
	float dist = nominator / denominator;

	if (dist < 0) {
        //std::cout << "r.o " << p0 << std::endl;
        //std::cout << "r.d " << p1 << std::endl;
		t0 = abs(dist);
		//float eased_dist = dist * 0.95;
		float eased_dist = glm::abs(dist) * 0.99;
		hit = p0 + eased_dist * (p1 - p0);
        n_vec = n;
		return  true;
	}

	return false;
};

