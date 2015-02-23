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

Pixel::Pixel(int xin, int yin, Camera* cam) {
	camera = cam;
    color = vec4(0.0, 0.0, 0.0, 0.0);
    x = (float)xin;
	y = (float)yin;
};

void Pixel::remap() {
	//remaps in several steps to screen space
    x = (2 * ((x + 0.5) / camera->width) - 1) * camera->angle * camera->aspect_ratio;
	y = (1 - 2.0 * ((y + 0.5) / camera->height)) * camera->angle;
    //x = (2 * (( x + 0.5 ) * INVWIDTH) - 1) * ANGLE * ASPECT_RATIO;
    //y = (1 - 2 * (( y + 0.5 ) * INVHEIGHT )) * ANGLE;
};

vec3 Pixel::map() {
    vec3 eye = vec3(0.0);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 center = vec3(0.0, 0.0, -1.0);

    vec3 w = glm::normalize(eye - center);
    vec3 u = glm::normalize(glm::cross(up, w));
    vec3 v = glm::cross(w, u);

    float half_width = camera->width / 2;
    float half_height = camera->height / 2;
    float alpha = tan(camera->fovx / 2.0) * ( (x - half_width) / half_width);
    float beta = tan(camera->fovy / 2.0) * ( (half_height - y) / half_height);

    vec3 dir = glm::normalize((alpha * u) + (beta * v) - w);
    return dir;
}

void Pixel::set_color(vec4 col) {
    color = col;
}

void Pixel::set_color(float nr, float ng, float nb, float na) {
    color.r = nr;
    color.g = ng;
    color.b = nb;
    color.a = na;
};

vec3 Pixel::convert_rgba_to_rgb(vec4 bgp) {
    //rgba -> rgb against bg or on white
    //white preserves original color
    vec3 rgb_color;

    rgb_color.r = ((1.0 - color.a) * bgp.r) + (color.a * color.r);
    rgb_color.g = ((1.0 - color.a) * bgp.g) + (color.a * color.g);
    rgb_color.b = ((1.0 - color.a) * bgp.b) + (color.a * color.b);

    return rgb_color;
}

void Pixel::add_alpha_color(vec4 color2) {
    //Add two rgba colors
    vec4 new_col;
    //float a = color.a + color2.a * (1 - color.a);
    //float r = (color.r * color.a + (color2.r * color2.a * (1 - color.a))) / a;
    //float g = (color.g * color.a + (color2.g * color2.a * (1 - color.a))) / a;
    //float b = (color.b * color.a + (color2.b * color2.a * (1 - color.a))) / a;

    float a = color2.a + color.a * (1 - color2.a);
    float r = (color2.r * color2.a + (color.r * color.a * (1 - color2.a))) / a;
    float g = (color2.g * color2.a + (color.g * color.a * (1 - color2.a))) / a;
    float b = (color2.b * color2.a + (color.b * color.a * (1 - color2.a))) / a;
    color = vec4(r, g, b, a);
}

