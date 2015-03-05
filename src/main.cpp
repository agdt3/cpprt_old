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
int HIT_COUNT = 0;
const int NUM_OBJECTS = 3;
const int MAX_REFLECTIONS = 1;
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

void object_setup() {
    mat4 tr = Transform::translate(0.0, 4.0, -18.0);
    Light *light1 = new Light(1.0, &tr, vec4(1.0, 1.0, 1.0, 0.5), 0.97, LightType::point);

	mat4 world = mat4(1.0);
	tr = Transform::translate(0.0, 1.0, -15.0);
	mat4 final = world * tr;

	//some local objects
    //Sphere sp1 = Sphere(1.0, &final, vec4(0.0, 0.0, 0.5, 1.0), 0.97);
    //Object *sphere1 = &sp1;
    Sphere *sphere1 = new Sphere(1.0, &final, vec4(0.0, 0.0, 0.5, 1.0), 0.97);

    tr = Transform::translate(-2.5, 1.0, -20.0);
    Sphere *sphere2 = new Sphere(2.0, &tr, vec4(0.0, 0.5, 0.5, 1.0), 0.97);

    Triangle *triangle1 = new Triangle(vec3(-2.0, 0.0, -25.0), vec3(0.0, 8.0, -12.0), vec3(3.0, -3.0, -5.0), vec4(1.0, 0.0, 0.0, 1.0));

    Plane *plane1 = new Plane(vec3(-1.0, -1.0, 0.0), vec3(1.0, -1.0, 0.0), vec3(0.0, -1.0, -1.0), vec4(0.0, 0.5, 0.0, 1.0));

	objects[0] = sphere1;
    objects[1] = light1;
    objects[2] = sphere2;
	//objects[2] = plane1;
	//objects[3] = triangle1;
}

void object_teardown() {
    for (int i = 0; i < NUM_OBJECTS; i++) {
        delete objects[i];
    }
}

void world_teardown(Camera *camera) {
    object_teardown();
    delete camera;
}

Camera* world_setup() {
    mat4 matv = mat4(1.0);
    mat4 *matp = &matv;
    Camera *camp = new Camera(matp, 640, 480, 45.0, 45.0, vec3(0.0));

    object_setup();
    return camp;
}

void trace_ray(Ray *ray, Pixel &pixel, int reflections){
    if (reflections > MAX_REFLECTIONS){
        return;
    }
    else {
        reflections++;
    }
    float nearest = INFINITY;

    for (int k = 0; k < NUM_OBJECTS; k++) {
	    float dist1 = INFINITY;
		float dist2 = INFINITY;
        vec3 hit = vec3(0.0);
		vec3 n = vec3(0.0);

		Object* obj = objects[k];

        if (obj->intersects(*ray, hit, n, dist1, dist2)) {
            HIT_COUNT++;
            if (dist1 < nearest) {
                nearest = dist1;
                if (ray->type == RayType::camera) {
                    pixel.set_color(obj->color);
                }
                else {
                    pixel.add_alpha_color(obj->color);
                }
            }
            //vec3 dir = Transform::reflect(ray.direction, n);
			//Ray nray = Ray(hit, dir, RayType::shadow);
            //trace_ray(nray, pixel, reflections);
        }
    }
}

void tracer(Camera *camera) {
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
            trace_ray(ray, pixel, reflections);

            //draw pixel
            vec3 rgb_color = pixel.convert_rgba_to_rgb(vec4(1.0));
			//cout << rgb_color.r << " " << rgb_color.g << " " << rgb_color.b << endl;
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


int main(int argc, char* argv[]) {
    //won't be doing rotation, translation, so init is fine, I think
	//mat4 camera_world = Transform::lookAt(eyeinit, centerinit, upinit);

    //init_objects();
	/*
    mat4 matv = mat4(1.0);
    mat4 *matp = &matv;
    Camera cam = Camera(matp, 640, 480, 45.0, 45.0, vec3(0.0));
    Camera *camp = &cam;

    FreeImage_Initialise();
	FIBITMAP* bitmap = FreeImage_Allocate(cam.width, cam.height, cam.bpp);

    mat4 tr = Transform::translate(0.0, 4.0, -18.0);
    Light l1 = Light(1.0, &tr, vec4(1.0, 1.0, 1.0, 0.5), 0.97, LightType::point); //wat??
    Light *light1 = &l1;

	mat4 world = mat4(1.0);
	tr = Transform::translate(0.0, 1.0, -15.0);
	//mat4 sc = Transform::scale(2, 2, 2);
	mat4 final = world * tr;

	//some local objects
    Sphere sp1 = Sphere(1.0, &final, vec4(0.0, 0.0, 0.5, 1.0), 0.97);
    Object *sphere1 = &sp1;

    tr = Transform::translate(0.0, 1.0, -20.0);
    Sphere sp2 = Sphere(2.0, &tr, vec4(0.0, 0.5, 0.5, 1.0), 0.97);
    Object *sphere2 = &sp2;

    Triangle tr1 = Triangle(vec3(-2.0, 0.0, -25.0), vec3(0.0, 8.0, -12.0), vec3(3.0, -3.0, -5.0), vec4(1.0, 0.0, 0.0, 1.0));
	Object *triangle1 = &tr1;

    Plane pl1 = Plane(vec3(-1.0, -1.0, 0.0), vec3(1.0, -1.0, 0.0), vec3(0.0, -1.0, -1.0), vec4(0.0, 0.5, 0.0, 1.0));
    Object *plane1 = &pl1;

	objects[0] = sphere1;
	//objects[1] = plane1;
    objects[1] = light1;
    //objects[2] = sphere2;
	//objects[2] = plane1;
	//objects[3] = triangle1;
    int count = 0;
    for (int i = 0; i < cam.width; i++) {
		for (int j = 0; j < cam.height; j++) {
			//base color
			RGBQUAD color;
			color.rgbRed = 0;
			color.rgbGreen = 0;
			color.rgbBlue = 0;

			//base pixel remapping
			Pixel pixel = Pixel(i, j, camp);
            pixel.set_color(vec4(0.0, 0.0, 0.0, 1.0));
			pixel.remap();

			//base camera ray
            //image is inversed here, I think ???
            //however this screws up the orientation of the plane, leading to the idea that the sphere intersection algorithm is wrong

            vec3 direction = vec3(pixel.x, pixel.y, -1); //direction of negative z
			//vec3 direction = pixel.map();
            direction = glm::normalize(direction);

            vec3 origin = vec3(0.0);
			Ray ray = Ray(origin, direction, RayType::camera);

			float tnear = INFINITY;

			float tmin = INFINITY;
			float tmax = INFINITY;
			for (int k = 0; k < NUM_OBJECTS; k++) {
				tmin = INFINITY;
				tmax = INFINITY;
				vec3 hit = vec3(0.0);
				vec3 n = vec3(0.0);

				Object* obj = objects[k];
				if (obj->intersects(&ray, &hit, &n, &tmin, &tmax)) {

                    //depth test
					//absolute distance, not possibly negative values!
					float dist = abs(tmin);
					if (dist < tnear) {
						tnear = dist;

                        bool isLight = false;
                        if (obj->type == ObjType::light) {
                            //we hit a light, paint it white
                            isLight = true;
                        }

                        vec3 dir = Transform::reflect(direction, n);
						dir = glm::normalize(dir);
                        Ray shadowRay = Ray(hit, dir, RayType::shadow);
						bool inShadow = true;

						vec3 light_hit, light_n;
						float light_t0, light_t1;
                        //cout << shadowRay.direction << endl;
                        //if (shadowRay.direction.y > 0.5)

                        //check if we can even reach the point light with this vector at all
                        if (light1->intersects(&shadowRay, &light_hit, &light_n, &light_t0, &light_t1)) {
							cout << "hit light" << endl;
                            cout << shadowRay.direction << endl;
                            inShadow = false;
							for (int l = 0; l <  NUM_OBJECTS; l++) {
								float nt1, nt2;
								vec3 new_hit=vec3(0.0), new_n=vec3(0.0);

                                float dist_to_light = abs(light_t0);
                                //cout << dist_to_light << endl;

                                if(objects[l]->intersects(&shadowRay, &new_hit, &new_n, &nt1, &nt2) && objects[l]->type != ObjType::light) {
                                    if (obj->type == ObjType::plane && objects[l]->type == ObjType::sphere) {
                                        //cout << "from plane intersects sphere" << endl;
                                        //cout << "x " << shadowRay.direction.x << " y " << shadowRay.direction.y << " z " << shadowRay.direction.z << endl;
                                        //cout << "nt1 " << nt1 << " dist to light " << dist_to_light << endl;
                                    }

                                    if (nt1 < dist_to_light) {
                                        inShadow = true;
                                        break;
                                    }
					            }
                            }
						}

                        //do we intersect with the base ray?

						//isInShadow = false;
						pixel.set_color(obj->color);
                        if (isLight) {
                            pixel.set_color(vec4(1.0, 1.0, 1.0, 1.0));
                        }
                        else if (inShadow) {
							//cout << "in shadow" << endl;
                            pixel.set_color(obj->color);
					    }
                        else {
							//may need a better additive model here
				            if (obj->type == ObjType::sphere) {
                                count++;
                                //cout << "light val " << i << " " << j << endl;
                            }
                            pixel.add_alpha_color(light1->color);
                            //cout << "after" << pixel << endl;
						}
                        vec3 rgb_color = pixel.convert_rgba_to_rgb(vec4(1.0));
			            //cout << rgb_color.r << " " << rgb_color.g << " " << rgb_color.b << endl;
                        color.rgbRed = (double)(rgb_color.r * 255.0);
		                color.rgbGreen = (double)(rgb_color.g * 255.0);
			            color.rgbBlue = (double)(rgb_color.b * 255.0);
					}
				}
			}
            // y value is inverted because pixel maps calculate with y=0 at the top left
            // but y = 0 at the bottom left for image representations
            FreeImage_SetPixelColor(bitmap, i, cam.height - j, &color);
		}
	}
    cout << count << endl;
    if (FreeImage_Save(FIF_PNG, bitmap, "./test/test.png", 0)) {
		cout << "Saved!";
	}
	FreeImage_DeInitialise();

    printf("Hit any key to continue> ");
    getchar();
    //system("pause");
    */
    //test_setup();

    Camera *camp = world_setup();
    tracer(camp);
    world_teardown(camp);
    cout << endl << "hits " << HIT_COUNT << " total " << (640 * 480) << endl;
    printf("Hit any key to continue> ");
    getchar();

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
