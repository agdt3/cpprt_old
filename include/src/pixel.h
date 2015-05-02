#include "transform.h"
#include "variables.h"
#include "camera.h"
#include <ostream>

#ifndef PIXEL_H
#define PIXEL_H

class Pixel
{
public:
	float x, y;
    vec4 color;
    Camera* camera;

    Pixel(int, int, Camera*);
	void remap();
    vec3 map();
    void set_color(vec4);
    void set_color(float, float, float, float);
    void add_alpha_color(vec4);
    vec3 convert_rgba_to_rgb(vec4 bg);

};

// toString representation of pixel
inline
std::ostream & operator<< (std::ostream &stream, Pixel const &p){
    stream << "x " << p.x << " y " << p.y << std::endl << "r " << p.color.r << " g " <<  p.color.g << " b " << p.color.b << " a " << p.color.a << std::endl; 
    return stream;
}
#endif
