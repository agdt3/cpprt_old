#include "src/light.hpp"
#include "src/transform.h"

//when camera vector makes a hit
//for all lights, check if light is reachable
//from that point, given that any point in sapce
//can be subject to a light vector
/*
vec3 LightManager::find_light_intersections (Object *lights, Object *objects, const& vec3 hit) {
    std::map <int, HitResult>;
    for (var i = 0; i < lights.length(); i++) {
        float center = lights[i]->center;
        vec3 light_vector = Light::build_light_vector(center, hit);
        if (!check_collisions(light_vector, hit, objects) {
            // do we want to push lights or light intersection results
            light_map.push_back(lights[i]);
            //add this light to the map of lights that strike this point
        }
    }
}

bool check_collisions(const& vec3 light_vector, const& vec3 hit, Object *objects) {
    //min dist is the distance between the hit origin and the intended light source
    var min_dist = math.sqrt(light.center - );
    for (var i = 0; i < objects.length(); i++) {
       if (
    }
}
*/
/*
vec3 Light::build_light_vector (
    const& vec3 hit,
    const& vec3 light_center) {
    return glm::normalize(light - hit);
}

float Light::angle_visible_refelcted(
    const& vec3 visible,
    const& vec3 reflected,
    bool use_halfangle) {
    
    // 
    //
}
vec3 Light::calculate_diffuse_intensity() {

}

*/
