#include <map>
#include <string>
#include "FreeImage/FreeImage.h"
#include "transform.h"

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

//This is a basic texture manager
//It can load an image serving as a texture and save a reference to it
//Given a u and a v value, it can return a pixel color from that image
//That's it for now, maybe additional improvements later

class TextureManager
{
public:
    static std::map<std::string, FIBITMAP*> texture_handler_map;
    TextureManager () {};
    //NOTE: Include custom destructor method to close all the file handles

    static FIBITMAP* load_image (const std::string&, int);
    static bool get_uv_pixel_color (vec3&, const std::string&, const float&, const float&);

};

#endif




