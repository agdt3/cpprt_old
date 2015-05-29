#include "transform.h"
#include "variables.h"
#include "ray.h"

#ifndef OBJECTS_H
#define OBJECTS_H

//TODO: Not currently used, but  perhaps should replace the struct in main
/*
struct HitStruct
{
    bool contact;   //did hit?
    vec3 hit;       //hit point
    vec3 n;         //n vector to hit area
    float t0;       //near hit distance
    float t1;       //far hit distance
};
*/
//This is a pure virtual class that allows
//us to create an array of pointers to various subclasses

class Object
{
public:
    static int id_generator;
    int id;

    ObjType type;
	vec4 color;
    bool has_texture;
    std::string texture_filepath;
    float easing_distance;
	mat4 objectToWorld, worldToObject;

	Object();
	Object(mat4*, vec4, float, bool, std::string);
	Object(mat4*, vec4, float, int); //testing constructor with id
	virtual bool intersects (const Ray&, vec3&, vec3&, float&, float&) =0;
};

class Sphere : public Object
{
public:
	vec4 center;
	float radius;
    //vec3 pole; //may need to be a vec4 in the future
    //vec3 equator;
    vec3 pole = vec3(0.0, 1.0, 0.0);
    vec3 equator = vec3(-1.0, 0.0, 0.0);

	Sphere(float, mat4*, vec4 col, float);
	Sphere(float, mat4*, vec4 col, float, bool, std::string);
	Sphere(float, mat4*, vec4 col, float, int); //testing constructor
    Sphere(const Sphere&); //copy
	bool intersects (const Ray&, vec3&, vec3&, float&, float&);
    float get_phi(const vec3&);
    float get_theta(const vec3&, const float&);
    float get_v(const float&);
    float get_u(const vec3&, const float&);
    void get_uv(const vec3&, float&, float&);
};

class Light : public Object
{
public:
	vec4 center;
    vec3 direction;
	float radius;
	LightType ltype;

	Light(float, mat4*, vec4, float, LightType);
	bool intersects (const Ray&, vec3&, vec3&, float&, float&);

	//bool intersects_point(const Ray*, vec3*, vec3*, float*, float*);
	//bool intersects_ambient(const Ray*, vec3*, vec3*, float*, float*);
	//bool intersects_direction(const Ray*, vec3*, vec3*, float*, float*);
};

class Triangle : public Object
{
public:
	vec3 v0, v1, v2, n; //the 3 locations, with v0 being the origin, n is the normal

	Triangle(vec3 A, vec3 B, vec3 C, vec4 col);
	bool intersects (const Ray&, vec3&, vec3&, float&, float&);
};

class Plane : public Object
{
public:
	//vec3 v0, v1, v2, n;
    vec3 n;
    float D;
	//Plane(vec3 A, vec3 B, vec3 C, vec4 col);
	Plane(vec3, float, vec4, float);
    bool intersects (const Ray&, vec3&, vec3&, float&, float&);
};
/*
class Polygon : public Object {
public:
	int num_faces; // number of faces
	int num_vertices_per_face[]; //number of verticies for each face
	int vertex_indecies[]; // vertex indecies the form up each
	vec4 verticies[];
};
*/
#endif

