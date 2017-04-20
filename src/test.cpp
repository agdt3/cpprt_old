#include <gtest/gtest.h>
#include <src/camera.h>
#include <src/ray.h>
#include <src/pixel.h>
#include <src/objects.h>
#include <iostream>


TEST(Transform, vectorsRotateCorrectly) {
    //rotate a around b
    vec3 a = vec3(0.0, 1.0, -1.0);
    vec3 b = vec3(0.0, 0.0, 1.0);
    float deg = 180;
    vec3 res = Transform::rotate_vec(deg, a, b);

    float TOLERANCE = 0.001;
    EXPECT_NEAR(res.x, 0.0, TOLERANCE);
    EXPECT_NEAR(res.y, -1.0, TOLERANCE);
    EXPECT_NEAR(res.z, -1.0, TOLERANCE);
    //still needs to be inversed

}

TEST(Transform, correctReflection) {
    //reflect a with b being the n vector
    vec3 a = vec3(0.0, 1.0, -1.0);
    vec3 b = vec3(0.0, 0.0, 1.0);
    // a - (2 * (dot(b, a) * b))
    // i - (2 * (dot(n, i) * n))
    // or equivalent to i - (2 * (dot(i, -n) * -n))
    vec3 res = Transform::reflect(a, b);

    float TOLERANCE = 0.001;
    EXPECT_NEAR(res.x, 0.0, TOLERANCE);
    EXPECT_NEAR(res.y, 1.0, TOLERANCE);
    EXPECT_NEAR(res.z, 1.0, TOLERANCE);
}

namespace {
class BaseTest: public ::testing::Test
{
protected:
    Camera* cam;
    mat4* world_matrix;
    float TOLERANCE = 0.003;

    BaseTest() {
        world_matrix = new mat4(1.0);
        cam = new Camera(world_matrix, 640, 480, 90.0, 45.0, vec3(0.0));
    }

    virtual ~BaseTest() {
        if (cam != NULL) {
            delete cam;
        }

        if (world_matrix != NULL) {
            delete world_matrix;
        }
    }

    virtual void SetUp() {}

    virtual void Teardown() {}

};

TEST_F(BaseTest, PixelValueRemapsCorrectlyUnderFOVX90) {
    // start by assuming that image is at (0,0) in the upper left corner
    // FOVX=90 is the basis here, and maps to 1.33/1
    // FOVX<90 produces smaller values

    Pixel pix = Pixel(0.0, 0.0, cam);
    pix.remap();
    EXPECT_NEAR(pix.x, -1.33, TOLERANCE);
    EXPECT_NEAR(pix.y, 1.0, TOLERANCE);

    pix.x = 639;
    pix.y = 0;
    pix.remap();
    EXPECT_NEAR(pix.x, 1.33, TOLERANCE);
    EXPECT_NEAR(pix.y, 1.0, TOLERANCE);

    pix.x = 0;
    pix.y = 479;
    pix.remap();
    EXPECT_NEAR(pix.x, -1.33, TOLERANCE);
    EXPECT_NEAR(pix.y, -1.0, TOLERANCE);

    pix.x = 639;
    pix.y = 479;
    pix.remap();
    EXPECT_NEAR(pix.x, 1.33, TOLERANCE);
    EXPECT_NEAR(pix.y, -1.0, TOLERANCE);
}

TEST_F(BaseTest, PixelValueRemapsCorrectlyUnderFOVX45) {
    cam->set_fovx(45.0);

    float TOLERANCE = 0.003;
    Pixel pix = Pixel(0.0, 0.0, cam);
    pix.remap();
    EXPECT_NEAR(pix.x, -0.5514, TOLERANCE);
    EXPECT_NEAR(pix.y, 0.4133, TOLERANCE);

    pix.x = 640;
    pix.y = 0;
    pix.remap();
    //xp -0.55143 yprime 0.413357
    EXPECT_NEAR(pix.x, 0.5514, TOLERANCE);
    EXPECT_NEAR(pix.y, 0.4133, TOLERANCE);

    pix.x = 0;
    pix.y = 480;
    pix.remap();
    EXPECT_NEAR(pix.x, -0.5514, TOLERANCE);
    EXPECT_NEAR(pix.y, -0.4133, TOLERANCE);

    pix.x = 640;
    pix.y = 480;
    pix.remap();
    EXPECT_NEAR(pix.x, 0.5514, TOLERANCE);
    EXPECT_NEAR(pix.y, -0.4133, TOLERANCE);
}


TEST_F(BaseTest, PixelColorsAddedCorrectly) {
    // add second color over the top of the first
    Pixel pix = Pixel(0, 0, cam);
    pix.set_color(vec4(0.0, 1.0, 0.0, 0.5));
    pix.add_alpha_color(vec4(0.0, 0.0, 1.0, 0.5));

    //a color2.a + color.a * (1 - color2.a);
    //r = (color2.r * color2.a + (color.r * color.a * (1 - color2.a))) / a
    float TOLERANCE = 0.00001;

    EXPECT_NEAR(pix.color.a, 0.75, TOLERANCE);
    EXPECT_NEAR(pix.color.r, 0.0, TOLERANCE);
    EXPECT_NEAR(pix.color.g, 0.333333, TOLERANCE);
    EXPECT_NEAR(pix.color.b, 0.666667, TOLERANCE);
}

} // namespace


TEST(Sphere, intersectionSingleAxis) {
    mat4 tr = Transform::translate(0.0, 0.0, -3.0);
    Sphere sp1 = Sphere(1.0, &tr, vec4(0.0, 0.0, 0.0, 1.0), 1.0);

    vec3 origin = vec3(0.0);
    vec3 direction = vec3(0.0, 0.0, -1.0);
    Ray ray = Ray(origin, direction, RayType::camera);
    Ray *rayp = &ray;

    vec3 hit, n;
    float dist1, dist2;
    bool is_hit = sp1.intersects(*rayp, hit, n, dist1, dist2);

    float TOLERANCE = 0.0001;
    EXPECT_EQ(is_hit, true);
    EXPECT_NEAR(dist1, 2, TOLERANCE);
    EXPECT_NEAR(dist2, 4, TOLERANCE);

    EXPECT_EQ(hit.x, 0.0);
    EXPECT_EQ(hit.y, 0.0);
    EXPECT_EQ(hit.z, -2.0);

    EXPECT_EQ(n.x, 0.0);
    EXPECT_EQ(n.y, 0.0);
    EXPECT_EQ(n.z, 1.0);
}

TEST(Sphere, intersectionDoubleAxisAtAngle) {
    mat4 tr = Transform::translate(0.0, 2.0, -3.0);
    Sphere sp1 = Sphere(1.0, &tr, vec4(0.0, 0.0, 0.0, 1.0), 1.0);

    vec3 origin = vec3(0.0);
    vec3 direction = glm::normalize(vec3(0.0, 1.0, -1.0));
    Ray ray = Ray(origin, direction, RayType::camera);
    Ray *rayp = &ray;

    vec3 hit = vec3(0.0);
    vec3 n = vec3(0.0);
    float dist1, dist2;
    bool is_hit = sp1.intersects(*rayp, hit, n, dist1, dist2);

    float TOLERANCE = 0.001;
    EXPECT_EQ(is_hit, true);
    //impact is at 2 * sqrt(2) = 2.83
    EXPECT_NEAR(dist1, 2.828, TOLERANCE); //tca - thc
    EXPECT_NEAR(dist2, 4.243, TOLERANCE); //tca + thc

    EXPECT_NEAR(hit.x, 0.0, TOLERANCE);
    EXPECT_NEAR(hit.y, 2.0, TOLERANCE);
    EXPECT_NEAR(hit.z, -2.0, TOLERANCE);

    EXPECT_NEAR(n.x, 0.0, TOLERANCE);
    EXPECT_NEAR(n.y, 0.0, TOLERANCE);
    EXPECT_NEAR(n.z, 1.0, TOLERANCE);
}

TEST(Sphere, intersectionDoubleAxis) {
    mat4 tr = Transform::translate(0.0, 3.0, -3.0);
    Sphere sp1 = Sphere(1.0, &tr, vec4(0.0, 0.0, 0.0, 1.0), 1.0);

    vec3 origin = vec3(0.0);
    vec3 direction = vec3(0.0, 1.0, -1.0);
    Ray ray = Ray(origin, direction, RayType::camera);
    Ray *rayp = &ray;

    vec3 hit, n;
    float dist1, dist2;
    bool is_hit = sp1.intersects(*rayp, hit, n, dist1, dist2);

    float TOLERANCE = 0.01;
    EXPECT_EQ(is_hit, true);
    // 2 * sqrt(2) + curvature
    EXPECT_NEAR(dist1, 3.24, TOLERANCE); //curvature of sphere means it's a bit further away
    EXPECT_NEAR(dist2, 5.24, TOLERANCE); //diameter = 2 and ray passes through center

    EXPECT_NEAR(hit.x, 0.0, TOLERANCE);
    EXPECT_NEAR(hit.y, 2.29, TOLERANCE);
    EXPECT_NEAR(hit.z, -2.29, TOLERANCE);

    vec3 norm_n = glm::normalize(vec3(0.0, -1.0, 1.0));
    EXPECT_NEAR(n.x, norm_n.x, TOLERANCE);
    EXPECT_NEAR(n.y, norm_n.y, TOLERANCE);
    EXPECT_NEAR(n.z, norm_n.z, TOLERANCE);
}

TEST(Sphere, intersectionTripleAxis) {
    mat4 tr = Transform::translate(3.0, 3.0, -3.0);
    Sphere sp1 = Sphere(1.0, &tr, vec4(0.0, 0.0, 0.0, 1.0), 1.0);

    vec3 origin = vec3(0.0);
    vec3 direction = vec3(1.0, 1.0, -1.0);
    Ray ray = Ray(origin, direction, RayType::camera);
    Ray *rayp = &ray;

    vec3 hit, n;
    float dist1, dist2;
    bool is_hit = sp1.intersects(*rayp, hit, n, dist1, dist2);

    float TOLERANCE = 0.01;
    EXPECT_EQ(is_hit, true);
    //impact is at least 2 * sqrt(2) = 2.83 + curvature
    EXPECT_NEAR(dist1, 4.196, TOLERANCE);
    EXPECT_NEAR(dist2, 6.196, TOLERANCE);

    EXPECT_NEAR(hit.x, 2.42, TOLERANCE);
    EXPECT_NEAR(hit.y, 2.42, TOLERANCE);
    EXPECT_NEAR(hit.z, -2.42, TOLERANCE);

    vec3 norm_n = glm::normalize(vec3(-1.0, -1.0, 1.0));
    EXPECT_NEAR(n.x, norm_n.x, TOLERANCE);
    EXPECT_NEAR(n.y, norm_n.y, TOLERANCE);
    EXPECT_NEAR(n.z, norm_n.z, TOLERANCE);
}

TEST(Sphere, hasCorrectUVValues) {
    mat4 tr = Transform::translate(0.0, 0.0, 0.0);
    Sphere sp1 = Sphere(1.0, &tr, vec4(0.0, 0.0, 0.0, 1.0), 1.0);
    float TOLERANCE = 0.01;

    //(0.0, 0.707, 0.707)
    vec3 n = glm::normalize(vec3(0.0, 1.0, 1.0));
    float expected_phi = 2.356;
    float expected_theta = 0.25;
    float expected_v = 2.356 / MPI;
    float expected_u = expected_theta;

    //acos(-1 * dot(n, pole)) = acos(-1 * dot((0, 0.707, 0.707), (0, 1, 0)))
    //acos(-0.707)
    //2.356 -> this is the wider of the two angles between these vectors
    //i.e. acos(0.707) = 0.785 = 45 deg, acos(-0.707) = 2.35 = ~135 deg
    float phi = sp1.get_phi(n);
    EXPECT_NEAR(phi, expected_phi, TOLERANCE);

    //acos(dot(equator, n) / sin(phi)) * 0.5 * 1/PI
    //acos(dot((-1, 0, 0), (0, 0.707, 0.707)) / 0.707) / 6.28
    //acos(0) = 1.5707 / 6.28 = 0.25
    float theta = sp1.get_theta(n, phi);
    EXPECT_NEAR(theta, expected_theta, TOLERANCE);

    //dot(cross(pole, equator), n) > 0 ? theta : 1 - theta
    float u = sp1.get_u(n, theta);
    EXPECT_NEAR(u, expected_u, TOLERANCE);

    //phi * 1/PI = 2.356 / 3.1415926
    float v = sp1.get_v(phi);
    EXPECT_NEAR(v, expected_v, TOLERANCE);

    //the whole function should also work
    float u2, v2;
    sp1.get_uv(n, u2, v2);
    EXPECT_NEAR(u2, expected_u, TOLERANCE);
    EXPECT_NEAR(v2, expected_v, TOLERANCE);
}

TEST(Light, intersectionTripleAxis) {
    mat4 tr = Transform::translate(3.0, 3.0, -3.0);
    Light lg1 = Light(1.0, &tr, vec4(0.0, 0.0, 0.0, 1.0), 1.0, LightType::point);

    vec3 origin = vec3(0.0);
    vec3 direction = vec3(1.0, 1.0, -1.0);
    Ray ray = Ray(origin, direction, RayType::camera);
    Ray *rayp = &ray;

    vec3 hit, n;
    float dist1, dist2;
    bool is_hit = lg1.intersects(*rayp, hit, n, dist1, dist2);

    float TOLERANCE = 0.01;
    EXPECT_EQ(is_hit, true);
    EXPECT_NEAR(dist1, 4.196, TOLERANCE);
    EXPECT_NEAR(dist2, 6.196, TOLERANCE);

    EXPECT_NEAR(hit.x, 2.42, TOLERANCE);
    EXPECT_NEAR(hit.y, 2.42, TOLERANCE);
    EXPECT_NEAR(hit.z, -2.42, TOLERANCE);

    vec3 norm_n = glm::normalize(vec3(-1.0, -1.0, 1.0));
    EXPECT_NEAR(n.x, norm_n.x, TOLERANCE);
    EXPECT_NEAR(n.y, norm_n.y, TOLERANCE);
    EXPECT_NEAR(n.z, norm_n.z, TOLERANCE);
}

TEST(Light, canBeIntersectedByReflectedShadowRay) {
    mat4 trsp = Transform::translate(0.0, 2.0, -3.0);
    Sphere sp1 = Sphere(1.0, &trsp, vec4(0.0, 0.0, 0.0, 1.0), 1.0);

    mat4 trl = Transform::translate(0.0, 3.0, 0.0);
    Light lg1 = Light(1.0, &trl, vec4(0.0, 0.0, 0.0, 1.0), 1.0, LightType::point);

    vec3 hit, light_hit, n, light_n;
    float dist1, dist2, light_dist1, light_dist2;
    float TOLERANCE = 0.01;

    vec3 origin = vec3(0.0);
    vec3 direction = vec3(0.0, 1.0, -1.0);
    Ray ray = Ray(origin, direction, RayType::camera);
    Ray *rayp = &ray;

    bool is_hit = sp1.intersects(*rayp, hit, n, dist1, dist2);
    EXPECT_EQ(is_hit, true);

    vec3 sh_origin = hit;
    vec3 sh_direction = Transform::reflect(direction, n);
    Ray shadow_ray = Ray(sh_origin, sh_direction, RayType::shadow);
    Ray *shadow_rayp = &shadow_ray;

    bool light_is_hit = lg1.intersects(*shadow_rayp, light_hit, light_n, light_dist1, light_dist2);

    EXPECT_EQ(light_is_hit, true);
    EXPECT_NEAR(light_dist1, 1.414, TOLERANCE);
    EXPECT_NEAR(light_dist2, 2.828, TOLERANCE);

    EXPECT_NEAR(light_hit.x, 0.0, TOLERANCE);
    EXPECT_NEAR(light_hit.y, 3.0, TOLERANCE);
    EXPECT_NEAR(light_hit.z, -1.0, TOLERANCE);

    vec3 norm_n = glm::normalize(vec3(0.0, 0.0, -1.0));
    EXPECT_NEAR(light_n.x, norm_n.x, TOLERANCE);
    EXPECT_NEAR(light_n.y, norm_n.y, TOLERANCE);
    EXPECT_NEAR(light_n.z, norm_n.z, TOLERANCE);
}

TEST(Light, intersectionToSphereCanBeBlockedByOwnSphereObject) {
    mat4 trsp = Transform::translate(0.0, 2.0, -3.0);
    Sphere sp1 = Sphere(1.0, &trsp, vec4(0.0, 0.0, 0.0, 1.0), 1.0);

    mat4 trl = Transform::translate(0.0, 3.0, -6.0);
    Light lg1 = Light(1.0, &trl, vec4(0.0, 0.0, 0.0, 1.0), 1.0, LightType::point);

    vec3 hit, light_hit, n, light_n;
    float dist1, dist2, light_dist1, light_dist2;

    vec3 origin = vec3(0.0);
    vec3 direction = vec3(0.0, 1.0, -1.0);
    Ray ray = Ray(origin, direction, RayType::camera);
    Ray *rayp = &ray;

    bool is_hit = sp1.intersects(*rayp, hit, n, dist1, dist2);
    EXPECT_EQ(is_hit, true);

    vec3 sh_origin = hit;
    vec3 sh_direction = Transform::reflect(direction, n);
    Ray shadow_ray = Ray(sh_origin, sh_direction, RayType::shadow);
    Ray *shadow_rayp = &shadow_ray;

    bool light_is_hit = lg1.intersects(*shadow_rayp, light_hit, light_n, light_dist1, light_dist2);

    EXPECT_EQ(light_is_hit, false);
}

TEST(Pixel, createsCorrectDirectionVector) {
    mat4 matv = mat4(1.0);
    mat4 *matp = &matv;
    Camera cam = Camera(matp, 640, 480, 45.0, 45.0, vec3(0.0));
    Camera *camp = &cam;

    Pixel pix = Pixel(320.0, 240.0, camp); //should be near (0,0)
    pix.remap();

    mat4 trsp = Transform::translate(0.0, 0.0, -3.0);
    Sphere sp1 = Sphere(1.0, &trsp, vec4(0.0, 0.0, 0.0, 1.0), 1.0);

    vec3 hit, n;
    float dist1, dist2;
    float TOLERANCE = 0.01;

    vec3 origin = vec3(0.0);
    vec3 direction = vec3(pix.x, pix.y, -1.0);
    Ray ray = Ray(origin, direction, RayType::camera);
    Ray *rayp = &ray;

    bool is_hit = sp1.intersects(*rayp, hit, n, dist1, dist2);
    EXPECT_EQ(is_hit, true);
    EXPECT_NEAR(direction.x, 0.0, TOLERANCE);
    EXPECT_NEAR(direction.y, 0.0, TOLERANCE);
    EXPECT_NEAR(direction.z, -1.0, TOLERANCE);
}

TEST(Pixel, createsCorrectAcuteReflectionVector) {
    mat4 matv = mat4(1.0);
    mat4 *matp = &matv;
    Camera cam = Camera(matp, 640, 480, 45.0, 45.0, vec3(0.0));
    Camera *camp = &cam;

    Pixel pix = Pixel(320.0, 100.0, camp); //should be near (0,1)
    pix.remap();

    mat4 trsp = Transform::translate(0.0, 0.0, -3.0);
    Sphere sp1 = Sphere(1.0, &trsp, vec4(0.0, 0.0, 0.0, 1.0), 1.0);

    mat4 trl = Transform::translate(0.0, 4.0, -2.0);
    Light lg1 = Light(1.0, &trl, vec4(0.0, 0.0, 0.0, 1.0), 1.0, LightType::point);

    vec3 hit, light_hit, n, light_n;
    float dist1, dist2, light_dist1, light_dist2;
    float TOLERANCE = 0.01;

    vec3 origin = vec3(0.0);
    vec3 direction = glm::normalize(vec3(pix.x, pix.y, -1.0));
    Ray ray = Ray(origin, direction, RayType::camera);
    Ray *rayp = &ray;

    bool is_hit = sp1.intersects(*rayp, hit, n, dist1, dist2);
    EXPECT_EQ(is_hit, true);
    EXPECT_NEAR(direction.x, 0.0, TOLERANCE);
    EXPECT_NEAR(direction.y, 0.234, TOLERANCE);
    EXPECT_NEAR(direction.z, -0.97, TOLERANCE);

    vec3 sh_origin = hit;
    vec3 sh_direction = Transform::reflect(direction, n);
    Ray shadow_ray = Ray(sh_origin, sh_direction, RayType::shadow);
    Ray *shadow_rayp = &shadow_ray;

    bool light_is_hit = lg1.intersects(*shadow_rayp, light_hit, light_n, light_dist1, light_dist2);
    EXPECT_EQ(light_is_hit, true);
}

TEST(Pixel, createsCorrectObtuseReflectionVector) {
    mat4 matv = mat4(1.0);
    mat4 *matp = &matv;
    Camera cam = Camera(matp, 640, 480, 45.0, 45.0, vec3(0.0));
    Camera *camp = &cam;

    Pixel pix = Pixel(320.0, 75.0, camp); //should be near (0,1)
    pix.remap();

    mat4 trsp = Transform::translate(0.0, 0.0, -3.0);
    Sphere sp1 = Sphere(1.0, &trsp, vec4(0.0, 0.0, 0.0, 1.0), 1.0);

    mat4 trl = Transform::translate(0.0, 3.0, -3.0);
    Light lg1 = Light(1.0, &trl, vec4(0.0, 0.0, 0.0, 1.0), 1.0, LightType::point);

    vec3 hit, light_hit, n, light_n;
    float dist1, dist2, light_dist1, light_dist2;
    float TOLERANCE = 0.01;

    vec3 origin = vec3(0.0);
    vec3 direction = glm::normalize(vec3(pix.x, pix.y, -1.0));
    Ray ray = Ray(origin, direction, RayType::camera);
    Ray *rayp = &ray;

    bool is_hit = sp1.intersects(*rayp, hit, n, dist1, dist2);
    EXPECT_EQ(is_hit, true);
    EXPECT_NEAR(direction.x, 0.0, TOLERANCE);
    EXPECT_NEAR(direction.y, 0.273, TOLERANCE);
    EXPECT_NEAR(direction.z, -0.97, TOLERANCE);

    vec3 sh_origin = hit;
    vec3 sh_direction = glm::normalize(Transform::reflect(direction, n));
    Ray shadow_ray = Ray(sh_origin, sh_direction, RayType::shadow);
    Ray *shadow_rayp = &shadow_ray;
    EXPECT_NEAR(sh_direction.x, 0.0, TOLERANCE);
    EXPECT_NEAR(sh_direction.y, 0.99, TOLERANCE);
    EXPECT_NEAR(sh_direction.z, -0.073, TOLERANCE);

    bool light_is_hit = lg1.intersects(*shadow_rayp, light_hit, light_n, light_dist1, light_dist2);
    EXPECT_EQ(light_is_hit, true);
}

TEST(LightIntersection, oppositeDirectionDoesNotHit) {
    mat4 trl = Transform::translate(0.0, 3.0, -10.0);
    Light lg1 = Light(1.0, &trl, vec4(0.0, 0.0, 0.0, 1.0), 1.0, LightType::point);

    vec3 light_hit, light_n;
    float light_dist1, light_dist2;

    vec3 sh_origin = vec3(0.0, 0.0, -10.0);
    vec3 sh_direction = vec3(0.0, -1.0, 0.0);
    Ray shadow_ray = Ray(sh_origin, sh_direction, RayType::shadow);
    Ray *shadow_rayp = &shadow_ray;

    bool light_is_hit = lg1.intersects(*shadow_rayp, light_hit, light_n, light_dist1, light_dist2);
    EXPECT_EQ(light_is_hit, false);
}

TEST(PlaneIntersection, PlaneIntersectsAndReflectsCorrectly) {
    vec3 hit, n;
    float dist1, dist2;
    bool is_hit;
    float TOLERANCE = 0.01;

    Plane pl1 = Plane(vec3(0.0, 1.0, 0.0), 3.0, vec4(0.0, 0.0, 0.0, 1.0), 1.0);

    // -y should hit
    vec3 origin = vec3(0.0);
    vec3 direction = glm::normalize(vec3(0.0, -1.0, -1.0));
    Ray ray = Ray(origin, direction, RayType::camera);
    Ray *rayp = &ray;

    // y, +x, -x should miss
    vec3 direction2 = glm::normalize(vec3(0.0, 1.0, -1.0));
    Ray miss_ray1 = Ray(origin, direction2, RayType::camera);
    Ray *mrayp1 = &miss_ray1;

    vec3 direction3 = glm::normalize(vec3(1.0, 0.0, -1.0));
    Ray miss_ray2 = Ray(origin, direction3, RayType::camera);
    Ray *mrayp2 = &miss_ray2;

    vec3 direction4 = glm::normalize(vec3(-1.0, 0.0, -1.0));
    Ray miss_ray3 = Ray(origin, direction4, RayType::camera);
    Ray *mrayp3 = &miss_ray3;

    // should miss
    is_hit = pl1.intersects(*mrayp1, hit, n, dist1, dist2);
    EXPECT_EQ(is_hit, false);

    is_hit = pl1.intersects(*mrayp2, hit, n, dist1, dist2);
    EXPECT_EQ(is_hit, false);

    is_hit = pl1.intersects(*mrayp3, hit, n, dist1, dist2);
    EXPECT_EQ(is_hit, false);

    //should hit
    is_hit = pl1.intersects(*rayp, hit, n, dist1, dist2);
    EXPECT_EQ(is_hit, true);
    EXPECT_EQ(n.x, 0.0);
    EXPECT_EQ(n.y, 1.0);
    EXPECT_EQ(n.z, 0.0);

    EXPECT_NEAR(dist1, 4.2426, TOLERANCE);
    EXPECT_NEAR(dist2, 4.2426, TOLERANCE);

    EXPECT_NEAR(hit.x, 0.0, TOLERANCE);
    EXPECT_NEAR(hit.y, -3.0, TOLERANCE);
    EXPECT_NEAR(hit.z, -3.0, TOLERANCE);

    Ray reflected = Ray(hit, Transform::reflect(direction, n), RayType::shadow);

    //normalized dir(0, 1, -1)
    EXPECT_NEAR(reflected.direction.x, 0.0, TOLERANCE);
    EXPECT_NEAR(reflected.direction.y, 0.707, TOLERANCE);
    EXPECT_NEAR(reflected.direction.z, -0.707, TOLERANCE);
}

TEST(PlaneIntersection, PlaneDoesNotIntersectReflectedRay) {
    vec3 hit, n;
    float dist1, dist2;
    bool is_hit;

    Plane pl1 = Plane(vec3(0.0, 1.0, 0.0), 3.0, vec4(0.0, 0.0, 0.0, 1.0), 1.0);

    vec3 origin = vec3(0.0, -2.99, 0.0);
    vec3 direction = glm::normalize(vec3(0.0, 1.0, 0.0));
    Ray ray = Ray(origin, direction, RayType::camera);
    Ray *rayp = &ray;

    is_hit = pl1.intersects(*rayp, hit, n, dist1, dist2);
    EXPECT_EQ(is_hit, false);
}

TEST(PlaneIntersection, PlaneReturnsCorrectIntersection) {
    vec3 hit, n, hit_pl, n_pl;
    float dist1, dist2, dist1_pl, dist2_pl;
    bool is_hit, is_hit_pl;

    mat4 trsp = Transform::translate(0.0, 0.0, -2.0);
    Sphere sp1 = Sphere(1.0, &trsp, vec4(0.0, 0.0, 0.0, 1.0), 1.0);

    Plane pl1 = Plane(vec3(0.0, 1.0, 0.0), 3.0, vec4(0.0, 0.0, 0.0, 1.0), 1.0);

    vec3 origin = vec3(0.0, 0.0, 0.0);
    vec3 direction = glm::normalize(vec3(0.0, -0.5, -1.0));
    Ray ray = Ray(origin, direction, RayType::camera);
    Ray *rayp = &ray;

    is_hit = sp1.intersects(*rayp, hit, n, dist1, dist2);
    is_hit_pl = pl1.intersects(*rayp, hit_pl, n_pl, dist1_pl, dist2_pl);
    EXPECT_EQ(is_hit, true);
    EXPECT_EQ(is_hit_pl, true);
    EXPECT_LT(dist1, dist1_pl);
    //std::cout  << dist1 << " " << dist1_pl << std::endl;
}

