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
    int method = 1;
    if (method == 1) {
        vec3 SC = vec3(center.x, center.y, center.z); //ignore w here for now
	    float SR2 = radius * radius;
        vec3 OC = SC - ray.origin;
        float L2OC = glm::dot(OC, OC);
        vec3 RD = glm::normalize(ray.direction); //should always be a normal
        float t_ca = glm::dot(OC, RD);
        if(t_ca < 0) return false;

	    float D2 = L2OC - pow(t_ca, 2);

        //std::cout << "SC " << SC << std::endl;
        //std::cout << "OC " << OC << std::endl;
        //std::cout << "L2OC " << L2OC << std::endl;
        //std::cout << "t_ca " << t_ca << std::endl;
        //std::cout << "D2 " << D2 << std::endl;
        // if the distance between the closest point to the sphere center on
        // the projected ray is greater than the radius, then the projected
        // ray is definitely outside the bounds of the sphere
        if(D2 > SR2) return false;

	    float T2HC = SR2 - D2;
        if (T2HC < 0) return false;

        float THC = sqrt(T2HC);
        t0 = t_ca - THC; //distance to point of impact
	    t1 = t_ca + THC; //distance to other side of sphere

        //std::cout << "T2HC " << T2HC << std::endl;
        //std::cout << "t0 " << t0 << std::endl;
        //std::cout << "t1 " << t1 << std::endl;

        float dist = easing_distance * t0;
	    hit = ray.origin + (dist * RD);

	    n = (hit - SC) / (float)radius;
        //std::cout << "hit " << hit << std::endl;
        //std::cout << "n " << n << std::endl;
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
    else if (method == 3) {
        vec3 center3 = vec3(center.x, center.y, center.z);
        vec3 L = ray.origin - center3;

        float b = glm::dot(ray.direction, L);
        float r1, r2;

        float det = (b * b) - (glm::dot(L, L)) + (radius * radius);
        bool hits = false;
        if (det < 0) return false;
        else if (det == 0) {
            r1 = r2 = -b + sqrt(det);
            hits = true;
        }
        else if (det > 0) {
            r1 = -b + sqrt(det);
            r2 = -b - sqrt(det);

            //std::cout << r1 << " " << r2 << std::endl;
            hits = true;

            if ((r1 < 0 && r2 < 0) || (r1 > 0 && r2 < 0) || (r2 > 0 && r1 < 0)) {
                //if r1 > 0 and r2 < 0, point is inside sphere
                //if r1 <0 && r2 <0, point is in fron of sphere or on some
                //other weird state
                // only r1 > 0 and r2 > 0 qualifies
                return false;
            }
        }

        // Are r1/r2 supposed to be abs b/c of distance, not roots
        //r1 should be the smaller of the two distances
        if (glm::abs(r1) > glm::abs(r2)) std::swap(r1, r2);

        if (hits) {
            t0 = r1;
            t1 = r2;

            float dist = easing_distance * abs(r1);
            hit = ray.origin + (dist * ray.direction);

            n = (hit - center3) / radius;

            return true;
        }
        else {
            return false;
        }
    }
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
    if (ltype == LightType::ambient) {
		//ambient light is reachable from anywhere, except from behind an object
		return true;
	}
    else if (ltype == LightType::point) {
	    int method = 1;
        if (method == 1) {
            vec3 SC = vec3(center.x, center.y, center.z); //ignore w here for now
            float SR2 = radius * radius;
            vec3 OC = SC - ray.origin;
            float L2OC = glm::dot(OC, OC);
            vec3 RD = glm::normalize(ray.direction); //should always be a normal
            float t_ca = glm::dot(OC, RD);
            if(t_ca < 0) return false;

            float D2 = L2OC - pow(t_ca, 2);

            if(D2 > SR2) return false;

            float T2HC = SR2 - D2;
            if (T2HC < 0) return false;

            float THC = sqrt(T2HC);
            t0 = t_ca - THC; //distance to point of impact
            t1 = t_ca + THC; //distance to other side of sphere

            float dist = easing_distance * t0;
            hit = ray.origin + (dist * RD);

            n = (hit - SC) / (float)radius;
            return true;
        }
        else if (method == 2) {
            //analytic solution
            /* math
            ray.origin = (0,0,0)
            ray.direction = (0, 1, -1)
            circle.center = (0, 3, -3)
            circle.radius = 1

            L = (0, -3, 3)
            a = 1 theoretically
            b = 2 * -4 = -8
            c = 18 - 1 = 17

            det = b*b - 4ac
            det = 64 - 68 < 0 !!!
            //clearly this method is flawed,
            //since an intersection should occur
            */

            vec3 center3 = vec3(center.x, center.y, center.z); //ignore w here for now
            vec3 L = ray.origin - center3;

            float a = glm::dot(ray.direction, ray.direction); //these should be unit vectors totalling 1
            float b = (double) 2.0 * glm::dot(ray.direction, L);
            float c = glm::dot(L, L) - pow(radius, 2);
            float r1, r2;

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
        else if (method == 3) {
            //analytic solution

            //method 3 math
            /**
                ray.origin = (0,0,0)
                norm dir = (0, 1, -1) / sqrt(2)
                ray.direction = (0, 0.707, -0.707)
                sphere.center = (0, 3, -3)
                sphere.radius = 1
                L = (0, -3, 3)
                b = 0 -2.121 - 2.121 = -4.242
                det = 18 - 18 + 1 = 1
                det > 0
                r1 = -(-2.121) + det = 2.121 + 1 = 3.121
                r2 = -(-2.121) - det = 2.121 - 1 = 1.121

                //opposite
                ray.origin = (0,0,0)
                norm dir = (0, -1, -1) / sqrt(2)
                ray.direction = (0, -0.707, -0.707)
                sphere.center = (0, 3, -3)
                sphere.radius = 1
                L = (0, -3, 3)
                b = 0 + 2.121 - 2.121 = 0
                det = 0 * 0 - 18 + 1 = -17;
                det < 0; No hit!
                this method is a bit screwed up because it returns
                non-sensical roots
                note: recalculate distance somehow?

                ray.origin = (0, 0, -10)
                ray.direction = (0, -1, 0)
                sphere.center = (0, 3, -10)
                sphere.radius = 1
                L = (0, -3, 0)
                b = 0 + 3 + 0 = 3
                det = 9 - 9 + 1
                det > 0 so it's a hit even though it shouldn't be!


                ray.origin = (0, 0, 0)
                ray.direction = (0, -1, 0)
                sphere.center = (0, 3, 0)
                sphere.radius = 1
                L = (0, -3, 0)
                b = 3
                det = 9 - 9 + 1 = 1
                r1 = -3 + sqrt(1) = -2
                r2 = -3 - sqrt(1) = -4
                //No hit, as it should be

            */

            vec3 center3 = vec3(center.x, center.y, center.z);
            vec3 L = ray.origin - center3;

            float b = glm::dot(ray.direction, L);
            float r1, r2;

            float det = (b * b) - (glm::dot(L, L)) + (radius * radius);
            bool hits = false;
            if (det < 0) return false;
            else if (det == 0) {
                r1 = r2 = -b + sqrt(det);
                hits = true;
            }
            else if (det > 0) {
                r1 = -b + sqrt(det);
                r2 = -b - sqrt(det);

                //std::cout << r1 << " " << r2 << std::endl;
                hits = true;

                if ((r1 < 0 && r2 < 0) || (r1 > 0 && r2 < 0) || (r2 > 0 && r1 < 0)) {
                    //if r1 > 0 and r2 < 0, point is inside sphere
                    //if r1 <0 && r2 <0, point is in fron of sphere or on some
                    //other weird state
                    // only r1 > 0 and r2 > 0 qualifies
                    return false;
                }
            }

            // Are r1/r2 supposed to be abs b/c of distance, not roots
            //r1 should be the smaller of the two distances
            if (glm::abs(r1) > glm::abs(r2)) std::swap(r1, r2);

            if (hits) {
                t0 = r1;
                t1 = r2;

                float dist = easing_distance * abs(r1);
                hit = ray.origin + (dist * ray.direction);

                n = (hit - center3) / radius;

                return true;
            }
            else {
                return false;
            }
        }

    }
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

