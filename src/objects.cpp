#include "src/objects.h"
#include "src/variables.h"
#include "src/transform.h"
#include <iostream>

//static member definition
int Object::id_generator = 0;

Object::Object () {
    id = id_generator++;
    objectToWorld = mat4(1.0);
	worldToObject = glm::inverse(objectToWorld);
	color = vec4(0.0, 0.0, 0.0, 1.0);
}

Object::Object (
    mat4 *otw,
    vec4 col,
    float ease_dist=1.0,
    bool has_text=false,
    std::string filepath="")
{
    id = id_generator++;
    objectToWorld = *otw;
    worldToObject = glm::inverse(objectToWorld);
    color = col;
    easing_distance = ease_dist;

    has_texture = has_text;
    texture_filepath = filepath;
}

//testing constructor with set id
Object::Object (mat4 *otw, vec4 col, float ease_dist, int new_id) {
    id = new_id;
    objectToWorld = *otw;
    worldToObject = glm::inverse(objectToWorld);
    color = col;
    easing_distance = ease_dist;
}

//Uses Object constructor by passing it parameters //superclass constructor executes first
Sphere::Sphere (float r, mat4 *otw, vec4 col, float ease_dist) : Object(otw, col, ease_dist) {
	type = ObjType::sphere;
	center =  objectToWorld * vec4(0.0, 0.0, 0.0, 1.0);
	radius = r; //define the radius explicitely, not as part of a transform, although really should

    //currently pole and equator are hardcoded
    //but should be modified based on sphere rotaion
}

Sphere::Sphere (float r, mat4 *otw, vec4 col, float ease_dist, bool has_text, std::string filename) : Object(otw, col, ease_dist, has_text, filename) {
	type = ObjType::sphere;
	center =  objectToWorld * vec4(0.0, 0.0, 0.0, 1.0);
	radius = r; //define the radius explicitely, not as part of a transform, although really should
}

//testing constructor with id
Sphere::Sphere (float r, mat4 *otw, vec4 col, float ease_dist, int new_id) : Object(otw, col, ease_dist, new_id) {
	type = ObjType::sphere;
	center =  objectToWorld * vec4(0.0, 0.0, 0.0, 1.0);
	radius = r;
    pole = vec3(0.0, 1.0, 0.0);
    equator = vec3(0.0, 0.0, 1.0);
}

//copy constructor
Sphere::Sphere (const Sphere& source) {
    color = source.color;
    objectToWorld = source.objectToWorld;
    easing_distance = source.easing_distance;

    type = source.type;
    center = source.center;
    radius = source.radius;
}

bool Sphere::intersects (const Ray &ray, vec3 &hit, vec3 &n, float &t0, float &t1) {
    int method = 1;
    if (method == 1) {
        vec3 SC = vec3(center.x, center.y, center.z); //ignore w here for now
        vec3 RD = glm::normalize(ray.direction); //should always be a normal
        float SR2 = radius * radius;
        vec3 OC = SC - ray.origin;
        float L2OC = glm::dot(OC, OC);
        std::cout << "SC " << SC << std::endl;
        std::cout << "RD " << RD << std::endl;
        std::cout << "SR2 " << SR2 << std::endl;
        std::cout << "OC " << OC << std::endl;
        std::cout << "L2OC " << L2OC << std::endl;

        float t_ca = glm::dot(OC, RD);
        //sphere located behind ray origin
        if(t_ca < 0) return false;

	    float D2 = L2OC - pow(t_ca, 2);

        // if the distance between the closest point to the sphere center on
        // the projected ray is greater than the radius, then the projected
        // ray is definitely outside the bounds of the sphere
        if(D2 > SR2) return false;

	    float T2HC = SR2 - D2;
        if (T2HC < 0) return false;

        std::cout << "T_CA " << t_ca << std::endl;
        std::cout << "D2 " << D2 << std::endl;
        std::cout << "T2HC " << T2HC << std::endl;
        //if the origin is inside the sphere of light, it counts as a hit
        if (L2OC < SR2) {
            t0 = 0.0;
            t1 = NULL; //TODO: Figure this out
            hit = ray.origin;
            n = (float)-1.0 * OC;
            return true;
        }

        float THC = sqrt(T2HC);
        t0 = t_ca - THC; //distance to point of impact
	    t1 = t_ca + THC; //distance to other side of sphere
        std::cout << "THC " << THC << std::endl;

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
    return false;
}

float Sphere::get_phi (const vec3& n) {
    return glm::acos(glm::dot(((float)-1.0 * n), pole));
    //return glm::acos(glm::dot(n, pole));
}

float Sphere::get_theta (const vec3& n, const float& phi) {
    //DIVPI is precalculated 1/PI
    return glm::acos(glm::dot(equator, n) / sin(phi)) * DIVPI * 0.5;
}

float Sphere::get_v (const float& phi) {
    return phi * DIVPI;
}

float Sphere::get_u (const vec3& n, const float& theta) {
    if (glm::dot(glm::cross(pole, equator), n) > 0) {
        return theta;
    } else {
        return 1.0 - theta;
    }
}

void Sphere::get_uv (const vec3& n, float& u, float& v) {
    float phi = get_phi(n);
    float theta = get_theta(n, phi);
    u = get_u(n, theta);
    v = get_v(phi);
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
    }
    return false;
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

Plane::Plane(vec3 n_vec, float dist, vec4 col, float ease_dist) {
    D = dist;
    n = n_vec;
    color = col;
    easing_distance = ease_dist;
    type = ObjType::plane;
}

bool Plane::intersects (const Ray &ray, vec3 &hit, vec3 &n_vec, float &t0, float &t1) {
    vec3 rd = glm::normalize(ray.direction);
    float vd = glm::dot(n, rd);

    // ray direction and plane n are perpendicular
    // therefore ray is parallel to plane
    if (vd == 0) return false;

    float v0 = -1.0 * (glm::dot(n, ray.origin) + D);

    float t = v0 / vd;

    //plane is behind point of origin, ignore
    if (t < 0) return false;

    t0 = t1 = t;
    float dist = easing_distance * glm::abs(t);
    hit = ray.origin + (dist * rd);
    n_vec = n;

    return true;
}

/*
void Plane::project_to_uv (std::vector<vec3> points) {
    float x = glm::abs(n.x);
    float y = glm::abs(n.y);
    float z = glm::abs(n.z);
    int discard;

    for (int i = 0; i < 3; ++i) {
        //
    }


    discard = max(max(x, y), z);

    if ((x == y && y == z) || (x == y && y > z) || (x == z && z > y)) {
        discard = 0;
    }
    else if (y == z && z > x) {
        discard = 1;
    }
    else if (x > y) {
        if (x > z) {
            discard = 0;
        } else {
            discard = 2;
        }
    }
    else if (y > z) {
        discard = 1;
    } else {
        discard = 2;
    }

    std::vector<vec3> remapped_points;
    for (int i = 0; i < points.size(); ++i) {
        vec3 v = points[i];
        remapped_points.pushback(points[i]);
    }
}

void Plane::derive_formula (const Ray &ray, vec3 aPoint) {
    //first hit calculates the intrinsic plane formula
    if (A != NULL && B != NULL && C != NULL) return;

    A = "";
    B = "";
    C = "";
}*/
