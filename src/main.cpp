//#include <math>
#include <cstdlib> //for system("pause")

#include <string>
#include <iostream>
#include "src/variables.h"
#include "src/main.h"
#include "FreeImage/FreeImage.h"
#include "src/transform.h"
#include "src/ray.h"
#include "src/pixel.h"
#include "src/objects.h"
#include "gtest/gtest.h"
#include "src/camera.h"

using namespace std;

//global
const bool RUN_TEST = true;
int HIT_COUNT = 0;
int LIGHT_HIT_COUNT = 0;
const bool LIGHT_VISIBLE = true;
const int NUM_OBJECTS = 3;
const int MAX_REFLECTIONS = 2;
Object* objects[NUM_OBJECTS];

/*
void init_objects() {
	for (int i=0; i < NUM_OBJECTS; i++) {
		//cout << (double (rand())/RAND_MAX) * 5 << endl;
		vec3 pos = vec3((double (rand())/RAND_MAX) * 5, (double (rand())/RAND_MAX) * 5, ((double (rand())/RAND_MAX) * -15) - 5);
		float radius = ((double (rand())/RAND_MAX) * 2) + 1.0;
		vec4 color = vec4((double (rand())/RAND_MAX), (double (rand())/RAND_MAX), (double (rand())/RAND_MAX), (double (rand())/RAND_MAX));
		mat4 id = mat4(1.0);
		Object* sphere = new Sphere(pos, radius, color, &id);
		objects[i] = sphere;
	}
}*/

void object_setup()
{
    mat4 tr = Transform::translate(-2.0, 3.0, -13.0);
    Light *light1 = new Light(2.0, &tr, vec4(1.0, 1.0, 1.0, 0.5), 0.97, LightType::point);

	mat4 world = mat4(1.0);
	tr = Transform::translate(0.0, 0.0, -15.0);
	mat4 final = world * tr;

    Sphere *sphere1 = new Sphere(1.0, &final, vec4(0.0, 0.0, 0.5, 1.0), 0.97);

    tr = Transform::translate(-1.5, -3.0, -15.0);
    Sphere *sphere2 = new Sphere(1.0, &tr, vec4(0.0, 0.5, 0.5, 1.0), 0.97);
/*
    Triangle *triangle1 = new Triangle(vec3(-2.0, 0.0, -25.0), vec3(0.0, 8.0, -12.0), vec3(3.0, -3.0, -5.0), vec4(1.0, 0.0, 0.0, 1.0));

    Plane *plane1 = new Plane(vec3(-1.0, -1.0, 0.0), vec3(1.0, -1.0, 0.0), vec3(0.0, -1.0, -1.0), vec4(0.0, 0.5, 0.0, 1.0));
*/

	objects[0] = sphere1;
    objects[1] = light1;
    objects[2] = sphere2;
	//objects[3] = plane1;
	//objects[3] = triangle1;
}

void object_teardown()
{
    for (int i = 0; i < NUM_OBJECTS; i++) {
        delete objects[i];
    }
}

void world_teardown(Camera *camera)
{
    object_teardown();
    delete camera;
}

Camera* world_setup()
{
    mat4 matv = mat4(1.0);
    mat4 *matp = &matv;
    Camera *camp = new Camera(matp, 1024, 768, 45.0, 45.0, vec3(0.0));

    object_setup();
    return camp;
}

struct Hit
{
    bool is_hit = false;
    vec3 hit, n;
    float d1, d2;
    ObjType type;
    vec4 color;
};

void trace_ray(Ray *ray, Pixel &pixel, int reflections, bool track=false)
{
    if (reflections >= MAX_REFLECTIONS) {
        return;
    }
    else {
        reflections++;
    }

    if (track) {
        cout << *ray << endl;
    }

    float nearest = INFINITY;
    float dist1, dist2;
    vec3 hit, n;
    Hit hit_result = Hit();

    for (int k = 0; k < NUM_OBJECTS; k++) {
	    Object* obj = objects[k];
        if (obj->intersects(*ray, hit, n, dist1, dist2)) {
            if (abs(dist1) < nearest) {
                //store results of hit here - probably should use a struct
                HIT_COUNT++;
                nearest = abs(dist1);
                hit_result.is_hit = true;
                hit_result.hit = hit;
                hit_result.n = n;
                hit_result.d1 = dist1;
                hit_result.d2 = dist2;
                hit_result.type = obj->type;
                hit_result.color = obj->color;
            }
        }
    }

    if (hit_result.is_hit) {
        if (ray->type == RayType::camera) {
            if (hit_result.type == ObjType::light && LIGHT_VISIBLE) {
                pixel.set_color(hit_result.color);
            }
            else if (hit_result.type != ObjType::light) {
                pixel.set_color(hit_result.color);

                //fire a new ray
                vec3 dir = (Transform::reflect(ray->direction, n));
                dir = glm::normalize(dir);
                Ray *nray = new Ray(hit, dir, RayType::shadow);
                trace_ray(nray, pixel, reflections, track);
                delete nray;
            }
        }
        else if (ray->type == RayType::shadow) {
            if (hit_result.type == ObjType::light) {
                LIGHT_HIT_COUNT++;
                pixel.add_alpha_color(hit_result.color);
            }
        }
    }
}

void tracer(Camera *camera)
{
    FreeImage_Initialise();
    FIBITMAP* bitmap = FreeImage_Allocate(camera->width, camera->height, camera->bpp);
    RGBQUAD color;

    for (int i = 0; i < camera->width; i++) {
	    for (int j = 0; j < camera->height; j++) {
			//base color
            color.rgbRed = 0.0;
	        color.rgbGreen = 0.0;
	        color.rgbBlue = 0.0;

			//base pixel remapping
			Pixel pixel = Pixel(i, j, camera);
			pixel.remap();
            pixel.set_color(vec4(0.0, 0.0, 0.0, 1.0));

            vec3 direction = vec3(pixel.x, pixel.y, -1.0); //direction of negative z
			direction = glm::normalize(direction);
			vec3 origin = vec3(0.0);

			//initial camera ray
            int reflections = 0;
            Ray *ray = new Ray(origin, direction, RayType::camera);

            bool track = false;
            if (i == (int)(camera->width/2) && j == 305) {
                track = true;
            }
            trace_ray(ray, pixel, reflections, track);

            //draw pixel
            vec3 rgb_color = pixel.convert_rgba_to_rgb(vec4(1.0));
            color.rgbRed = (double)(rgb_color.r * 255.0);
		    color.rgbGreen = (double)(rgb_color.g * 255.0);
			color.rgbBlue = (double)(rgb_color.b * 255.0);
			FreeImage_SetPixelColor(bitmap, i, camera->height-j, &color);

            delete ray;
        }
    }
    if (FreeImage_Save(FIF_PNG, bitmap, "./test/test.png", 0)) {
		cout << "Saved!";
	}
	FreeImage_DeInitialise();
}


int main(int argc, char* argv[])
{
    Camera *camp = world_setup();
    tracer(camp);
    world_teardown(camp);
    cout << endl << "hits " << HIT_COUNT << " total " << (camp->width * camp->height) << endl;
    cout << "light hits " << LIGHT_HIT_COUNT << endl;

    printf("Hit any key to continue> ");
    getchar();

    if (RUN_TEST) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    } else {
        return 0;
    }
}
