#include "src/texture.hpp"
#include <iostream>
//static
std::map<std::string, FIBITMAP*> TextureManager::texture_handler_map;

//static
//NOTE: load_image code adapted from FreeImaage manual
FIBITMAP* TextureManager::load_image (const std::string& imagepath, int flag) {
    //checkt the map first
    auto it = texture_handler_map.find(imagepath);
    if (it != texture_handler_map.end()) {
        return it->second;
    }
    std::cout<<"had to load texture"<< std::endl;
    //load
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    const char* path = imagepath.c_str();

    fif = FreeImage_GetFileType(path, 0);
    if (fif == FIF_UNKNOWN) {
        fif = FreeImage_GetFIFFromFilename(path);
    }

    if ((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
        //TODO:I don't know the scope of this pointer
        FIBITMAP *file = FreeImage_Load(fif, path, flag);
        texture_handler_map.insert(std::pair<std::string, FIBITMAP*> (imagepath, file));
        return file;
    }
    return NULL;
}

//static
bool TextureManager::get_uv_pixel_color (vec3& rgb, const std::string& imagepath, const float& u, const float& v) {
    if (imagepath == "") {
        return false;
    }

    //lookup every time you need to set a value is bad
    FIBITMAP* file = load_image(imagepath, 0);
    if (file != NULL) {
        unsigned int width = FreeImage_GetWidth(file);
        unsigned int height = FreeImage_GetHeight(file);
        unsigned int x = width * u;
        unsigned int y = height * v;
        RGBQUAD *color;
        if (FreeImage_GetPixelColor(file, x, y, color)) {
            rgb.r = color->rgbRed / 255.0;
            rgb.g = color->rgbGreen / 255.0;
            rgb.b = color->rgbBlue / 255.0;
            return true;
        } else {
            return false;
        }
    }
    return false;
}

