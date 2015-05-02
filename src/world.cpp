#include "src/world.h"
#include "src/variables.h"
#include "src/camera.h"
#include "src/objects.h"

World::World ()
{
    /*
    mat4 matv = mat4(1.0);
    mat4 *matp = &matv;
    _camera = new Camera(matp, 1024, 768, 45.0, 45.0, vec3(0.0));
    */
    /*
    _objects = new Object[num_objects];
    mat4 tr = Transform::translate(-2.0, 3.0, -13.0);
    Light *light1 = new Light(2.0, &tr, vec4(1.0, 1.0, 1.0, 0.5), 0.97, LightType::point);

	mat4 world = mat4(1.0);
	tr = Transform::translate(0.0, 0.0, -15.0);
	mat4 final = world * tr;

    Sphere *sphere1 = new Sphere(1.0, &final, vec4(0.0, 0.0, 0.5, 1.0), 0.97);

    tr = Transform::translate(-2.0, -0.75, -15.0);
    Sphere *sphere2 = new Sphere(1.0, &tr, vec4(0.0, 0.5, 0.7, 1.0), 0.97);

    Triangle *triangle1 = new Triangle(vec3(-2.0, 0.0, -25.0), vec3(0.0, 8.0, -12.0), vec3(3.0, -3.0, -5.0), vec4(1.0, 0.0, 0.0, 1.0));

    Plane *plane1 = new Plane(vec3(-1.0, -3.0, 0.0), vec3(1.0, -3.0, 0.0), vec3(0.0, -3.0, -1.0), vec4(0.0, 0.5, 0.0, 1.0));

	_objects[0] = sphere1;
    _objects[1] = light1;
    _objects[2] = sphere2;
	_objects[3] = plane1;
	//objects[4] = triangle1;*/
}

World::~World()
{
    /*
    for(int i = 0; i < num_objects; i++) {
        delete _objects[i];
    }

    delete _camera;
    */
}


