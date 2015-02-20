#include <gtest/gtest.h>
#include <src/ray.h>
#include <src/objects.h>

TEST(remapPixel, remapsCorrectly) {
    // start by assuming that image is at (0,0) in the upper left corner
    /*
    int WIDTH = 640;
    int HEIGHT = 480;
    float FOV = 90.0;
    float ANGLE = tan(Transform::degToRad(FOV/2));
    float ASPECT_RATIO = (float)WIDTH / (float)HEIGHT;
    //x = (((2*(0+0.5)) / 640) - 1) * 0.7853 * 1.33
    //y = (1 - (2 * (0 + 0.5) / 480)) * 0.785
    */
    
    //TODO: refactor global variables into camera object
    float FOVX = 90.0;
    float ANGLE = tan(Transform::degToRad(FOVX/2));
    float TOLERANCE = 0.003;
    Pixel pix = Pixel(0.0, 0.0);
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

TEST(colorPixel, colorsAddedCorrectly) {
    // add second color over the top of the first
    Pixel pix = Pixel(0, 0);
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

TEST(transform, vectorsRotateCorrectly) {
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

TEST(transform, correctReflection) {
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

TEST(sphere, intersectionSingleAxis) {
    mat4 tr = Transform::translate(0.0, 0.0, -3.0);
    Sphere sp1 = Sphere(1.0, &tr, vec4(0.0, 0.0, 0.0, 1.0));

    vec3 origin = vec3(0.0);
    vec3 direction = vec3(0.0, 0.0, -1.0);
    Ray ray = Ray(origin, direction, RayType::camera);

    vec3 hit;
    vec3 n;
    float dist1;
    float dist2;
    bool is_hit = sp1.intersects(&ray, &hit, &n, &dist1, &dist2);
    
    float TOLERANCE = 0.0001;
    float DISTANCE_EASING = 0.97;
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

TEST(sphere, intersectionDoubleAxis) {
    mat4 tr = Transform::translate(0.0, 3.0, -3.0);
    Sphere sp1 = Sphere(1.0, &tr, vec4(0.0, 0.0, 0.0, 1.0));

    vec3 origin = vec3(0.0);
    vec3 direction = vec3(0.0, 1.0, -1.0);
    Ray ray = Ray(origin, direction, RayType::camera);

    vec3 hit;
    vec3 n;
    float dist1;
    float dist2;
    bool is_hit = sp1.intersects(&ray, &hit, &n, &dist1, &dist2);
    
    float TOLERANCE = 0.01;
    float DISTANCE_EASING = 0.97;
    EXPECT_EQ(is_hit, true);
    EXPECT_NEAR(dist1, 2.29, TOLERANCE);
    EXPECT_NEAR(dist2, 4.29, TOLERANCE);
    /* 
    EXPECT_EQ(hit.x, 0.0);
    EXPECT_EQ(hit.y, 0.0);
    EXPECT_EQ(hit.z, -2.0);

    EXPECT_EQ(n.x, 0.0);
    EXPECT_EQ(n.y, 0.0);
    EXPECT_EQ(n.z, 1.0);
    */
}
